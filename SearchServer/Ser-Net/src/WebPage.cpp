#include "WebPage.hpp"

#include <stdlib.h>

#include <iostream>
#include <queue>
#include <algorithm>
#include <sstream>

using std::cout;
using std::endl;
using std::priority_queue;
using std::pair;
using std::make_pair;
using std::istringstream;


namespace wd {
//根据pair<,>第二个元素词频降序
struct WordFreqCompare {
  bool operator()(const pair<string, int> & left, const pair<string, int> & right) {
    if (left.second < right.second){
      return true;	
    }
    else if (left.second == right.second && left.first > right.first) {	
      return true;	
    }
    else {	
      return false;	
    }
  }
};


WebPage::WebPage(string & doc, Configuration & config, WordSegmentation & jieba) {
  //cout << "WebPage()" << endl;
  _topWords.reserve(TOPK_NUMBER);   //top词集向量大小根据设定初始化
  processDoc(doc, config, jieba);   //处理该篇string形式的文章
}

//传入一篇文档进行处理,获取各部分信息，并生成该文章无停顿词的top词集
void WebPage::processDoc(const string & doc, Configuration & config, WordSegmentation & jieba) {
  //string形式文章中各项的分割符
  string docIdHead = "<docid>";
  string docIdTail = "</docid>";
  string docUrlHead = "<link>";
  string docUrlTail = "</link>";
  string docTitleHead = "<title>";
  string docTitleTail = "</title>";
  string docContentHead = "<content>";
  string docContentTail = "</content>";
  
  //从string形式文章中提取各部分信息
  // docid 
  int bpos = doc.find(docIdHead);
  int epos = doc.find(docIdTail);
  string docId = doc.substr(bpos + docIdHead.size(),  //相对位置得到区间内容 
                            epos - bpos - docIdHead.size());
  _docId = atoi(docId.c_str());
  //cout << "========" << endl << _docId << endl;
  
  // title
  bpos = doc.find(docTitleHead);
  epos = doc.find(docTitleTail);
  _docTitle = doc.substr(bpos + docTitleHead.size(), 
			 epos - bpos - docTitleHead.size());
  //cout << "========" << endl << _docTitle << endl;
  
  // url
  bpos = doc.find(docUrlHead);
  epos = doc.find(docUrlTail);
  _docUrl = doc.substr(bpos + docUrlHead.size(), 
		       epos - bpos - docUrlHead.size());
  //cout << "========" << endl << _docUrl << endl;

  // content
  bpos = doc.find(docContentHead);
  epos = doc.find(docContentTail);
  _docContent = doc.substr(bpos + docContentHead.size(),
			   epos - bpos - docContentHead.size());
  //cout << "========" << endl << _docContent << endl;


  //对文章内容content进行分词后保存
  vector<string> wordsVec = jieba(_docContent.c_str());
  
  //获取配置中的停顿词
  set<string>& stopWordList = config.getStopWordList();

  //调用函数生成无停顿词的Top词集
  calcTopK(wordsVec, TOPK_NUMBER, stopWordList);  //参(文章内容vec, 词集大小参数, 停顿词集set)
}


void WebPage::calcTopK(vector<string> & wordsVec, int k, set<string> & stopWordList) {
  //统计文章中每个非停顿词的词频
  for (auto iter = wordsVec.begin(); iter != wordsVec.end(); ++iter) {
    auto sit = stopWordList.find(*iter);
    if (sit == stopWordList.end()) {
      ++_wordsMap[*iter];  //private data wordfre_map<string, ++int>
    }
  }
  //使用 priority_queue<> 对文章词频统计map的每个元素进行 WordFreqCompare函数 方式的排序
  priority_queue<pair<string, int>, vector<pair<string, int> >, WordFreqCompare> wordFreqQue(_wordsMap.begin(), _wordsMap.end());
  
  //对排序后的词频vec截取top词集
  while (!wordFreqQue.empty()) {
    string top = wordFreqQue.top().first;
    wordFreqQue.pop();
    //忽略掉单个字母 /r回车 /n换行(ASIIC)
    if (top.size() == 1 && (static_cast<unsigned int>(top[0]) == 10 ||
	                    static_cast<unsigned int>(top[0]) == 13)) {
      continue;	
    }
    _topWords.push_back(top);
    //只截取设定数量的词
    if (_topWords.size() >= TOPK_NUMBER) {
      break;
    }
  }

//获取词集失败则打印两个容器内容(词频map和top词集vec)
#if 0
  for (auto mit : _wordsMap) {
    cout << mit.first << "\t" << mit.second << std::endl;	
  }
  cout << endl;

  for (auto word : _topWords) {
    cout << word << "\t" << word.size() << "\t" << static_cast<unsigned int>(word[0]) << std::endl;
  }
#endif
}

// 根据top词集判断两篇文档是否相同
bool operator == (const WebPage & lhs, const WebPage & rhs) {
  int commNum = 0;
  auto lIter = lhs._topWords.begin();
  for (;lIter != lhs._topWords.end(); ++lIter) {
    commNum += std::count(rhs._topWords.begin(), rhs._topWords.end(), *lIter);  //比较两个top词集相同个数
  }
  
  int lhsNum = lhs._topWords.size();
  int rhsNum = rhs._topWords.size();
  int totalNum = lhsNum < rhsNum ? lhsNum : rhsNum;                             //两个top词集个数取小者

  if ( static_cast<double>(commNum) / totalNum > 0.75 )                         //阈值大于0.75
    return true;	
  
  else 
    return false;	
  
}

//对文档按照docId进行排序
bool operator < (const WebPage & lhs, const WebPage & rhs) {
  if(lhs._docId < rhs._docId)
    return true;
  else 
    return false;
}

//生成摘要
string WebPage::summary(const vector<string> & queryWords) {
  vector<string> summaryVec;
  istringstream iss(_docContent);  //以文档内容作为构建iss对象
  string line;
  //每行逐一查找是否包含关键字，包含则将该行放入vec
  while (iss >> line) {
    for (auto word : queryWords) {
      if (line.find(word) != string::npos) {  //string::npos特殊值意为串A中无串B
        summaryVec.push_back(line);
	break;
      }
    }
    if (summaryVec.size() >= 5)  //找到五行
      break;	
    
  }
  //拼成一个字符串返回
  string summary;
  for (auto line : summaryVec) {
    summary.append(line).append("\n");
  }	
  return summary;
}


}// end of namespace wd
