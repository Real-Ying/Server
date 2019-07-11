#ifndef _WD_WEB_PAGE_H_
#define _WD_WEB_PAGE_H_

#include "Configuration.hpp"
#include "WordSegmentation.hpp"

#include <string>
#include <map>
#include <set>
#include <vector>

using std::string;
using std::map;
using std::vector;
using std::set;

namespace wd {

//网页...对象

class WebPage {
  friend bool operator == (const WebPage & lhs, const WebPage & rhs);        //根据top词集判断两篇文章(WebPage实例)是否相同的=重载
  friend bool operator < (const WebPage & lhs, const WebPage & rhs);         //按docoid对数篇文章排序的外部功能的<重载
 public:
  const static int TOPK_NUMBER = 20;                                         //截取的top词数量

  WebPage(string & doc, Configuration & config, WordSegmentation & jieba);   //构造函数,提取文章信息并生成top词集

  string summary(const vector<string> & queryWords);                         //生成摘要
  
  //几个常量成员函数,获取网页库实例各项信息
  int getDocId() {    
    return _docId;	
  }
  map<string, int> & getWordsMap() {
    return _wordsMap;	
  }
  string getTitle() {
    return _docTitle;	
  }
  string getUrl() {
    return _docUrl;	
  }

 private:
  void processDoc(const string & doc, Configuration & config, WordSegmentation & jieba);   //对string形式得文章提取各项信息并生成top词集

  void calcTopK(vector<string> & wordsVec, int k, set<string> & stopWordList);             //计算top词集

 private:
  int    _docId;        //文档id
  string _docTitle;     //文档标题
  string _docUrl;	//文档URL
  string _docContent;	//文档内容
  string _docSummary;   //文档摘要，由summary生成

  vector<string>   _topWords;   //top词集
  map<string, int> _wordsMap;   //存储文档的所有词及其词频(去停用词之后)
};

}// end of namespace wd

#endif
