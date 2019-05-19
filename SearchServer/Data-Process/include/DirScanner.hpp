
#ifndef _WD_DIRSCAN_H_
#define _WD_DIRSCAN_H_

#include <string>
#include <vector>

namespace wd {

const int kFileNo = 10000;

class Configuration;

class DirScanner {
 public:
  DirScanner(Configuration & conf);            //构造函数

  void operator()();                           //重载()调用traverse函数

  std::vector<std::string> & files();          //返回_vecFilesfiles的引用

  void debug();                                //打印vecFiles_ 所有语料文件路径

 private:
  void traverse(const std::string & dirName);  //获取某一目录下的所有文件
 private:
  Configuration & conf_;               //配置文件对象引用
  std::vector<std::string> vecFiles_;  //存放每个语料文件的绝对路径
};

} // end of namespace wd
#endif
