
#include "WordQuery.hpp"
#include "GlobalDefine.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <json/json.h>

using std::cout;
using std::endl;
using std::ifstream;
using std::stringstream;

namespace wd {
  
  //相似度比较结构体(余弦算法)
  struct SimilarityCompare {
    SimilarityCompare(vector<double> & base)
        : _base(base)   // 传入base基准向量（X）
    {}

    bool operator()(const pair<int, vector<double> > & lhs, //Y1 Y2
          	    const pair<int, vector<double> > & rhs) {	
      double lhsCrossProduct = 0;
      double rhsCrossProduct = 0;
      double lhsVectorLength = 0;
      double rhsVectorLength = 0;
		
      for (int index = 0; index != _base.size(); ++index) {     
        lhsCrossProduct += (lhs.second)[index] * _base[index];  // Y1*X Y2*X
        rhsCrossProduct += (rhs.second)[index] * _base[index];
        lhsVectorLength += pow((lhs.second)[index], 2);         //Y1^2 Y2^2
        rhsVectorLength += pow((rhs.second)[index], 2);
      }

      if (lhsCrossProduct / sqrt(lhsVectorLength) < rhsCrossProduct / sqrt(rhsVectorLength)) {  //两个 cosθ = (Y*X) / |Y| 比较 	
        return false;	
      }
      else {	
        return true;	
      }
    }

    vector<double> _base;  //基准向量

};

//构造函数
WordQuery::WordQuery(Configuration & conf) 
    : _conf(conf) {
  
  loadLibrary();  //执行载入3个文件库函数

}

//装载三个数据库文件进容器
void WordQuery::loadLibrary() {
  //新网页库和新位置偏移库放入文件输入流缓存区
  ifstream ifsOffset(_conf.getConfigMap()[NEWOFFSETLIB_KEY].c_str());
  ifstream ifsPage(_conf.getConfigMap()[NEWPAGELIB_KEY].c_str());
  if(!(ifsOffset.good() && ifsPage.good())) {
    cout << "offset page lib ifstream open error!" << endl;
  }
  
  //读位置偏移库的各项 和 并以此读取文章内容  
  int docid, offset, length;
  string line;
  int cnt = 0;
  while (getline(ifsOffset, line)) {
    stringstream ss(line);
    ss >> docid >> offset >> length;

    string doc;
    doc.resize(length, ' ');
    ifsPage.seekg(offset, ifsPage.beg);
    ifsPage.read(&*doc.begin(), length);
    
    //经webpage处理得到新网页库容器和新位置偏移库容器
    WebPage webPage(doc, _conf, _jieba);
    _pageLib.insert(make_pair(docid, webPage));  //新网页库放入的是webpage对象，可获取文章各项和 词频 top词集

    _offsetLib.insert(make_pair(docid, make_pair(offset, length)));  //新位置偏移库仅形式变化
//  if(++cnt == 3)
//    break;
  }
  ifsOffset.close();
  ifsPage.close();
#if 0
  for (auto item : _offsetLib) {
    cout << item.first << "\t" << item.second.first << "\t" << item.second.second << endl;
  }
#endif
  // 装载倒排索引表
  ifstream ifsInvertIndex(_conf.getConfigMap()[INVERTINDEX_KEY].c_str());
  if (!ifsInvertIndex.good()) {
    cout << "invert index ifstream open error!" << endl;
  }

  string word;
  double weight;
  cnt = 0;
  while (getline(ifsInvertIndex, line)) {
    stringstream ss(line);
    ss >> word;
    set<pair<int, double> > setID;
    while (ss >> docid >> weight) {
      setID.insert(make_pair(docid, weight));
    }
    _invertIndexTable.insert(make_pair(word, setID)); //unordered_map(pair<word, pair<doic, weight>> )

//  if(++cnt == 21)
//    break;
  }
  ifsInvertIndex.close();

  cout << "loadLibrary() end" << endl;

//for debug
#if 0
  for (auto item : _invertIndexTable) {
    cout << item.first << "\t";
    for (auto sitem : item.second) {
      cout << sitem.first << "\t" << sitem.second << "\t";
    }
    cout << endl;
  }
#endif
}

string WordQuery::doQuery(const string & str) {  //传入用户输入的字符串
  //对输入字符串进行分词，并预先查找是否在库中存在
  vector<string> queryWords;
  if (str.size() > 0) {
    queryWords = _jieba(str.c_str());  //使用jieba对象进行分词
  }

  for (auto item : queryWords) {  //查询输入中每个词的的倒排索引结果
    cout << item << '\n';
    auto uit = _invertIndexTable.find(item);
    if (uit == _invertIndexTable.end()) {
      cout << "can not found " << item << endl;
      return returnNoAnswer();    //返回Json形式的 404
    }
  }
  cout << endl;

  //计算每个词的权重，并进行余弦相似度排序
  vector<double> weightList = getQueryWordsWeightVector(queryWords);   //获得查询串中每个词的权重
  SimilarityCompare similarityCmp(weightList);                         //余弦排序

  //执行查询,并将查询结果按余弦排序
  vector<pair<int, vector<double> > > resultVec;//保存查询结果的容器 vec( int(docid) vec<double>)所在行数
  if (executeQuery(queryWords, resultVec)) {    //执行查询函数
    stable_sort(resultVec.begin(), resultVec.end(), similarityCmp);  
    
    vector<int> docIdVec;   //保存所在文章的编号 vec
    for (auto item : resultVec) {
      docIdVec.push_back(item.first);
    }

    return createJson(docIdVec, queryWords);  //将查询结果(docid的vec)封装成Json数据
  }
  else {
    return returnNoAnswer();
  }
}

//Json形式的  error 404
string WordQuery::returnNoAnswer() {
  Json::Value root;
  Json::Value arr;

  Json::Value elem;
  elem["title"] = "404, not found";
  elem["summary"] = "亲,I cannot find what you want. What a pity!";
  elem["url"] = "";
  arr.append(elem);
  root["files"] = arr;
  Json::StyledWriter writer;
  return writer.write(root);
}

//计算查询词的weight
vector<double> WordQuery::getQueryWordsWeightVector(vector<string> & queryWords) {
  map<string, int> wordFreqMap;
  for (auto item : queryWords) {
    ++wordFreqMap[item];  //根据查询串中词的个数初始化词频map
  }
  //计算查询串中每个词的权重并返回
  vector<double> weightList;
  double weightSum = 0;
  int totalPageNum = _offsetLib.size();  //文章总数 

  for (auto & item : queryWords) {       //计算查询串中的每个词对之于网络库中所有文章的权重
    int df = _invertIndexTable[item].size();
    double idf = log(static_cast<double>(totalPageNum) / df + 0.05) / log(2); 
    int tf = wordFreqMap[item];
    double w = idf * tf;
    weightSum += pow(w, 2);
    weightList.push_back(w);            // w
  }
 
  for (auto & item : weightList) {      // 归一化
    item /= sqrt(weightSum);
  }

  cout << "weightList's elem: ";
  for (auto item : weightList) {
    cout << item << "\t";
  }
  cout << endl;
  return weightList;                    //返回weightList
}

//执行查询
bool WordQuery::executeQuery(const vector<string> & queryWords,      //传入一个查询输入和一个结果输出
		             vector<pair<int, vector<double> > > & resultVec) {
  cout << "executeQuery()" << endl;
  if (queryWords.size() == 0) {         //空串
    cout <<"empty string not find" << endl;
    return false;
  }
  /***********得到最小进度条的长度 和 各个进度条(带遍历起始位置和迭代次数)********************/  
  //保存求交集的迭代器 指向的对象格式：< iterator-><docid,weight>, iter_times >
  typedef set<pair<int, double> >::iterator setIter;	//pair<docid, weight>
  vector<pair<setIter, int> > iterVec;
 
  int minIterNum = 0x7FFFFFFF;        //求交集的最小迭代数的初始化(设为整数的大小上限是为了不断找到更小的长度直到等于最短的进度条长度)

  //遍历查询串中每个词，得到minIterNum(出现次数最少的那个关键词的出现数量 即最短的进度条长度) 和 统计每个词迭代次数的计数vector
  //最小迭代次数 意为各集合中docid相同的元素集合(交集)的长度不会超过最短的那个集合的长度
  for (auto item : queryWords) {
    int sz = _invertIndexTable[item].size();  
    if (sz == 0)                       //有关键词未出现在文章中就白搭
      return false;		
    if (minIterNum > sz)               //不断找到更小的size 
      minIterNum = sz;               

    iterVec.push_back(make_pair(_invertIndexTable[item].begin(), 0));  //将 迭代器(指向每个关键词的权重set头部)与初始迭代次数 的pair存为一个vec
  }                                                                    //大小为关键词数量
  cout << "minIterNum = " << minIterNum << endl;
  /*********************************************************************************************/

 
  /**********************************找到包含所有关键词的所有文章**********************************/

  bool isExiting = false;  //当isExiting=true说明所有符合条件的文章都已找出
 
  while (!isExiting) {
    //遍历iterVec，对进度条们逐层邻近两两判断每层所有关键词的docid是否相同(全部相同则记录或存在不同则break处理) 
    int idx = 0;    //每次找到一组后又从各进度条第一位开始找并计数，直到计数满(达到最小进度条长度)为止
    for (; idx != iterVec.size() - 1; ++idx) {  
      if ((iterVec[idx].first)->first != iterVec[idx+1].first->first)  
        break;
    }

    if (idx == iterVec.size() - 1) {	// 若此竖层docid全部相同(都是同一篇文章中)
      vector<double> weightVec;             //记录此竖层的同一个docid和所有weight
      int docid = iterVec[0].first->first;  //
      //
      for (idx = 0; idx != iterVec.size(); ++idx) {
        weightVec.push_back(iterVec[idx].first->second);  
        ++(iterVec[idx].first);                            //每一个迭代器(setIter)++(开始判断下一层)
        ++(iterVec[idx].second);                           //找到一篇符合条件得文章，计数器++
        if (iterVec[idx].second == minIterNum) {	//若判断迭代次数已经到达最小迭代数(最短集合的长度)则查找完毕信号量置true 然后返回  
          isExiting = true;	
        } 
      }
      resultVec.push_back(make_pair(docid, weightVec)); //交集vector <pair<docid, vector<weight...>>, ...>
    }
    else {	                       //若此层有不同那就不合格，这层直接跳下层操作就行，多余动作
      int minDocId = 0x7FFFFFFF;
      int iterIdx;//保存minDocId的迭代器位置
      for (idx = 0; idx != iterVec.size(); ++idx) {  //遍历不全相同的这层，记录这层最小的docid(没必要)
        if (iterVec[idx].first->first < minDocId) {
          minDocId = iterVec[idx].first->first;
	  iterIdx = idx;
        }
      }
    
      ++(iterVec[iterIdx].first);                      //迭代器跳到下层，迭代器不计数
      ++(iterVec[iterIdx].second);
      if (iterVec[iterIdx].second == minIterNum) {
        isExiting = true;	
      }
    }
  }
  return true;
}

//数据封装成Json形式
string WordQuery::createJson(vector<int> & docIdVec, const vector<string> & queryWords) {
  Json::Value root;
  Json::Value arr;

  int cnt = 0;
  for (auto id : docIdVec) {
    string summary = _pageLib[id].summary(queryWords); //elem[queryword] = webpage
    string title = _pageLib[id].getTitle();            
    string url = _pageLib[id].getUrl();

    Json::Value elem;                                 
    elem["title"] = title;
    elem["summary"] = summary;
    elem["url"] = url;
    arr.append(elem);                                 //elem -> arr
    if (++cnt == 100)// 最多记录100条
      break;
  }

  root["files"] = arr;                                //将查询词和其得到的文章(webpage)返回
  Json::StyledWriter writer;
  return writer.write(root);
}

}// end of namespace wd
