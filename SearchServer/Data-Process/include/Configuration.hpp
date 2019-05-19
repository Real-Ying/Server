#ifndef _WD_CONFIGURATION_H_
#define _WD_CONFIGURATION_H_

#include <set>
#include <map>
#include <string>

namespace wd {

class Configuration {
 public:
  Configuration(const std::string & filepath);          //构造函数

  std::map<std::string, std::string> & getConfigMap();  //获取存放配置文件内容的map
  std::set<std::string> & getStopWordList();            //获取停用词词集 
  void debug();
 
 private:
  void readConfiguration();                       //构造函数体运行的功能，将配置文件内容以map容器形式存储
 private:
  std::string filepath_;                          //配置文件路径
  std::map<std::string, std::string> configMap_;  //配置文件内容
  std::set<std::string> stopWordList_;            //停用词词集
};

}//end of namespace wd

#endif
