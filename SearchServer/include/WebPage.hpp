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

namespace wd
{


class WebPage {     
  //判断两篇文档是否相等
  friend bool operator == (const WebPage & lhs, const WebPage & rhs); 
  //对文档按Docid进行排序
  friend bool operator < (const WebPage & lhs, const WebPage & rhs);
 public:
  const static int TOPK_NUMBER = 20;   //k

  //构造函数 
  WebPage(string & doc, Configuration & config, WordSegmentation & jieba);
	
  int getDocId() {                    //获取文档的docid
    return _docId;	
  }
  map<string, int> & getWordsMap() {  //获取文档的词频统计map
    return _wordsMap;	
  }
  string getDoc() {	              //获取文档
    return _doc;	
  }

 private:
  //对已格式化文档进行处理（经PageLibc处理）
  void processDoc(const string & doc, Configuration & config, WordSegmentation & jieba);
  //求取文档的topk词集
  void calcTopK(vector<string> & wordsVec, int k, set<string> & stopWordList);

 private:
  string _doc;          //已格式化的文档

  int    _docId;        //文档id
  string _docTitle;     //文档标题
  string _docUrl;       //文档URL
  string _docContent;	//文档内容
  string _docSummary;   //文档摘要，需要自动生成，不是确定的

  map<string, int> _wordsMap; // 存储文档的所有词(去停用词之后)和该项词的词频
  vector<string>   _topWords; //topk词集vector<"word1", "word2", "word3", ~, "wordn">
};

}// end of namespace wd

#endif
