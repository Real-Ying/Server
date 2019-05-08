
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
  vector<string> wordsVec = jieba(_docContent.c_str());           //对content进行分词存于vector<string> wordVec(针对中文)
  set<string> & stopWordList = config.getStopWordList();          //获取配置中的停顿词
  calcTopK(wordsVec, TOPK_NUMBER, stopWordList);                  //求取文章的topk词集

}


//calcTopK()
void WebPage::calcTopK(vector<string> & wordsVec, int k, set<string> & stopWordList) {
  for (auto iter = wordsVec.begin(); iter != wordsVec.end(); ++iter) {
    auto sit = stopWordList.find(*iter);
    if (sit == stopWordList.end()) {
        ++_wordsMap[*iter];    //Map<string, int>保存文档中每个词及其词频
    }
  }

	
  priority_queue<pair<string, int>, vector<pair<string, int> >, WordFreqCompare>
  wordFreqQue(_wordsMap.begin(), _wordsMap.end());

  while(!wordFreqQue.empty()) {
    string top = wordFreqQue.top().first;
    wordFreqQue.pop();
    if (top.size() == 1 && (static_cast<unsigned int>(top[0]) == 10
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
bool operator == (const WebPage & lhs, const WebPage & rhs) 
{
	int commNum = 0;
	auto lIter = lhs._topWords.begin();
	for(;lIter != lhs._topWords.end(); ++lIter)
	{
		commNum += std::count(rhs._topWords.begin(), rhs._topWords.end(), *lIter);
	}
	
	int lhsNum = lhs._topWords.size();
	int rhsNum = rhs._topWords.size();
	int totalNum = lhsNum < rhsNum ? lhsNum : rhsNum;

	if( static_cast<double>(commNum) / totalNum > 0.75 )
	{	return true;	}
	else 
	{	return false;	}
}

//对文档按照docId进行排序
bool operator < (const WebPage & lhs, const WebPage & rhs)
{
	if(lhs._docId < rhs._docId)
		return true;
	else 
		return false;
}

}// end of namespace wd
