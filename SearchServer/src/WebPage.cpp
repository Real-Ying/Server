
#include "WebPage.hpp"

#include <stdlib.h>
#include <iostream>
#include <queue>
#include <algorithm>

using std::cout;
using std::endl;
using std::priority_queue;
using std::pair;
using std::make_pair;


namespace wd
{

//词语及其词频vector<pair<string, int>>的排序所需的重载，根据pair<>的第二个元素降序
struct WordFreqCompare
{
	bool operator()(const pair<string, int> & left, const pair<string, int> & right)
	{
		if(left.second < right.second)
		{	return true;	}
		else if(left.second == right.second && left.first > right.first)
		{	return true;	}
		else
		{	return false;	}
	}
};



WebPage::WebPage(string & doc, Configuration & config, WordSegmentation & jieba)  //构造函数 (传入格式化后的文档、配置、分词类对象)
    : _doc(doc) {                        //输入的这篇文档由PageLib.cc输出
  //cout << "WebPage()" << endl;
  _topWords.reserve(10);                 //存储一篇文章词频最高的前20个词的向量预留空间(改20？)
  processDoc(doc, config, jieba);        //对已格式化的文章进行处理

}

void WebPage::processDoc(const string & doc, Configuration & config, WordSegmentation & jieba) {       
  //用变量表示文档中每篇文章每项的开头和结尾的字符串标志
  string docIdHead = "<docid>";
  string docIdTail = "</docid>";
  string docUrlHead = "<link>";
  string docUrlTail = "</link>";
  string docTitleHead = "<title>";
  string docTitleTail = "</title>";
  string docContentHead = "<content>";
  string docContentTail = "</content>";

  //提取文档的docid
  int bpos = doc.find(docIdHead);                             //string::find(string) 查找返回与给定字符串序列相等的第一子串位置(首字符pos) 
  int epos = doc.find(docIdTail);
  string docId = doc.substr(bpos + docIdHead.size(),          //string::substr(pos, pos+count) 返回此区间子串
			    epos - bpos - docIdHead.size());  //由于使用的相对位置，第一个位置参数看作0，第二个参数是比第一个参数位数增加的数量
  _docId = atoi(docId.c_str());                               //atoi(char) 将char型数字串转化为整型

  //title
  bpos = doc.find(docTitleHead);
  epos = doc.find(docTitleTail);
  _docTitle = doc.substr(bpos + docTitleHead.size(), 
			 epos - bpos - docTitleHead.size());
  
  //content
  //cout << "================================================" << endl << _docTitle << endl;
  bpos = doc.find(docContentHead);
  epos = doc.find(docContentTail);
  _docContent = doc.substr(bpos + docContentHead.size(),
			   epos - bpos - docContentHead.size());

  //cout << "================================================" << endl << _docContent << endl;


  //
  vector<string> wordsVec = jieba(_docContent.c_str());           //对content进行分词存于vector<string> wordVec(针对中文) <"word1","word2", ...,"wordn">
  set<string> & stopWordList = config.getStopWordList();          //获取配置中的停顿词
  calcTopK(wordsVec, TOPK_NUMBER, stopWordList);                  //求取文章的topk词集

}


//calcTopK()
void WebPage::calcTopK(vector<string> & wordsVec, int k, set<string> & stopWordList) {
  //统计非停顿词的词频
  for (auto iter = wordsVec.begin(); iter != wordsVec.end(); ++iter) {  
    auto sit = stopWordList.find(*iter);      //set::find()无结果则返回set::end()
    if (sit == stopWordList.end()) {          //如果不是停顿词则该词词频++ 
        ++_wordsMap[*iter];    //Map<string, int>保存文档中每个词及其词频
    }
  }

  //对词及词频的map进行排序 //priority_queue<数据类型, 数组类容器, 比较方式的函数> 排列对象;
  //类型注意pair<,>，容器默认Vector,比较方式WordFreqCompare默认operate<重载
  priority_queue<pair<string, int>, vector<pair<string, int> >, WordFreqCompare> wordFreqQue(_wordsMap.begin(), _wordsMap.end());
  
  //获取topk词集
  while(!wordFreqQue.empty()) {
    string top = wordFreqQue.top().first;
    wordFreqQue.pop();
    //取词时过滤掉单个字母词、回车/r、换行/n
    if (top.size() == 1 && (static_cast<unsigned int>(top[0]) == 10  //这里static_cast<>将元素由string型强转数字整型size_t
		        || static_cast<unsigned int>(top[0]) == 13)) {	
        continue;
    }

    _topWords.push_back(top);
    if (_topWords.size() >= static_cast<size_t>(k)) {
	break;
    }
}


#if 0
	for(auto mit : _wordsMap)
	{
		cout << mit.first << "\t" << mit.second << std::endl;	
	}
	cout << endl;

	for(auto word : _topWords)
	{
		cout << word << "\t" << word.size() << "\t" << static_cast<unsigned int>(word[0]) << std::endl;
	}
#endif
}

// 判断两篇文档是否相同
bool operator == (const WebPage & lhs, const WebPage & rhs) {
  int commNum = 0;
  auto lIter = lhs._topWords.begin();
  for (;lIter != lhs._topWords.end(); ++lIter) {  //top词集中相同个数
    commNum += std::count(rhs._topWords.begin(), rhs._topWords.end(), *lIter);
  }
	
  int lhsNum = lhs._topWords.size();  //top词集数总量取两者更小
  int rhsNum = rhs._topWords.size();
  int totalNum = lhsNum < rhsNum ? lhsNum : rhsNum;

  if ( static_cast<double>(commNum) / totalNum > 0.75 ) {  //阈值0.75
      return true;
  }
  else {
      return false;
  }
}

//对文档按照docId进行排序(外部功能)
bool operator < (const WebPage & lhs, const WebPage & rhs) {
  if (lhs._docId < rhs._docId)
      return true;
  else 
      return false;
}

}// end of namespace wd
