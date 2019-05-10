#ifndef _WD_PAGE_LIB_PREPROCESSOR_H_
#define _WD_PAGE_LIB_PREPROCESSOR_H_


#include "WebPage.hpp"
#include "WordSegmentation.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::map;
using std::unordered_map;
using std::set;

using namespace wd;

namespace wd {

class PageLibPreprocessor {
 public:
  PageLibPreprocessor(Configuration & conf);  //构造函数

  void doProcess();                           //执行预处理

 private:
  void readInfoFromFile();                    //根据配置信息读取网页库和位置偏移库的内容

  void cutRedundantPages();                   //去重冗余的网页数据

  void buildInvertIndexTable();               //执行创建倒排索引

  void storeOnDisk();

 private:
  Configuration &                                  _conf;              //配置文件类对象的引用
  WordSegmentation                                 _jieba;             //分词库类对象

  vector<WebPage>                                  _pageLib;           //经PageLib类格式化后的网页数据库(容器)
  unordered_map<int, pair<int, int> >              _offsetLib;         //网页数据库的偏移库(容器)
  unordered_map<string, vector<pair<int, double>>> _invertIndexTable;  //倒排索引库(容器)
};

}// end of namespace wd


#endif
