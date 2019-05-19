
#include "Configuration.hpp"
#include "GlobalDefine.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

using std::cout;
using std::endl;

namespace wd {

Configuration::Configuration(const std::string & filepath)
    : filepath_(filepath) {
  readConfiguration();
}

//逐行将配置文件内容(what : where)以map[key]=value的形式存储
void Configuration::readConfiguration() {
  std::ifstream ifs(filepath_.c_str());
  if (ifs.good()) {
    std::string line;
    while (getline(ifs, line)) {
      std::istringstream iss(line);
      std::string key;
      std::string value;
      iss >> key >> value;
      configMap_[key] = value;
    }
    std::cout << "read configuration is over!" << std::endl;
  }
  else {
    std::cout << "ifstream open error" << std::endl;
  }
}

//获取以map容器存储的配置内容(配置容器在Configuration构造时就已生成)
std::map<std::string, std::string> & Configuration::getConfigMap() {
  return configMap_;
}

//获取以set容器存储的停用词集
std::set<std::string> & Configuration::getStopWordList() {
  //停用词词集set容器已有内容则直接提取 
  if (stopWordList_.size() > 0)  
    return stopWordList_;
  //否则在路径map容器中提取文件路径构建文件输入流，从中逐行(逐个)插入set容器
  std::string stopWordFilePath = configMap_[STOP_WORD_KEY];  
  std::ifstream ifs(stopWordFilePath.c_str());
  if (!ifs) {
    std::cout << "StopWord ifstream open error!" << std::endl;
}
	
  std::string word;
  while (getline(ifs, word)) {
    stopWordList_.insert(word);
}

// for debug	
#if 0
  for (auto &str : stopWordList_) {
    std::cout << str << "\t" << str.size() << std::endl;
  }
  std::cout << std::endl;
#endif

  return stopWordList_;
}

//打印配置文件的key --> value 内容
void Configuration::debug() {
  for (auto & elem : configMap_) {
    cout << elem.first << " --> " << elem.second << endl;
  }
}

}// end of namespace wd
