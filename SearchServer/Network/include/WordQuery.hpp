
#ifndef _WD_WORD_QUERY_H_
#define _WD_WORD_QUERY_H_

#include "WebPage.hpp"
#include "Configuration.hpp"
#include "WordSegmentation.hpp"

#include <vector>
#include <map>
#include <set>
#include <string>

using std::vector;
using std::map;
using std::unordered_map;
using std::set;
using std::string;
using std::pair;
using std::make_pair;
using namespace wd;

namespace wd {

class WordQuery {
 public:
  WordQuery(Configuration & conf);

  string doQuery(const string & str);

 private:
  void loadLibrary();                     //加载两个个数据库文件，经webpage生成新网页库(包含词频和top词集)和新位置偏移库

  vector<double> getQueryWordsWeightVector(vector<string> & queryWords);    //计算查询词的权重值weight
	
  bool executeQuery(const vector<string> & queryWords, vector<pair<int, vector<double> > > & resultVec);    //执行查询(结果保存resultVec)
	
  string createJson(vector<int> & docIdVec, const vector<string> & queryWords);   //结果的Json封装函数

  string returnNoAnswer();                //返回空结果的Json

 private:
  WordSegmentation                                _jieba;             //jieba分词对象
  Configuration &				  _conf;              //配置文件的引用(读取三个数据文件)
  unordered_map<int, WebPage>                     _pageLib;           //网页库
  unordered_map<int, pair<int, int> >             _offsetLib;         //偏移库
  unordered_map<string, set<pair<int, double> > > _invertIndexTable;  //倒排索引表
};

} // end of namespce wd
#endif
