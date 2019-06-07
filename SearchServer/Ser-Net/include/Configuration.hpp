
#ifndef _WD_CONFIGURATION_H_
#define _WD_CONFIGURATION_H_

#include <set>
#include <map>
#include <string>

namespace wd {

class Configuration {
 public:
  Configuration(const std::string & filepath);             //构造函数

  std::map<std::string, std::string> & getConfigMap();     //获取配置map容器
  std::set<std::string> & getStopWordList();               //获取停顿词set容器

 private:

  void readConfiguration();                         //读取配置
 
 private:
  std::string filepath_;                            //文件路string
  std::map<std::string, std::string> configMap_;    //存配置的map
  std::set<std::string> stopWordList_;              //停顿词set
};

}//end of namespace wd

#endif
