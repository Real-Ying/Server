
#include "DirScanner.hpp"
#include "Configuration.hpp"
#include "GlobalDefine.hpp"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <dirent.h>    

#include <iostream>

using std::cout;
using std::endl;

namespace wd {

DirScanner::DirScanner(Configuration & conf)
    : conf_(conf) {
  vecFiles_.reserve(kFileNo);  //保存语料文件路径的vec大小预留
}

//获取语料绝对路径，并调用trsverse()
void DirScanner::operator()() {
  std::map<std::string, std::string> & confMap = conf_.getConfigMap();  //由配置对象的引用获取配置内容map容器
  std::string dirName = confMap[YULIAO_KEY];  //从配置容器中得到语料的绝对路径
  traverse(dirName);  
}

//get private:vecFiles_ 语料文件路径(vec容器)
std::vector<std::string> & DirScanner::files() {
  return vecFiles_;
}

//打印vecFiles_ 所有语料文件路径
void DirScanner::debug() {
  for (auto & elem : vecFiles_) {
     cout << elem << endl;
  }
}


void DirScanner::traverse(const std::string & dirName) {
  //打开指定的目录
  DIR * pdir = ::opendir(dirName.c_str()); //::linux系统函数， DIR *opendir(const char *name)成功返指向Dir结构体的指针 失败NULL 
  if (NULL == pdir) {                  //#include <sys/Types.h> #include <dirent.h> ，返回的指针对象相当于目录标识符，使得其他相关函数能对这个目录操作
    printf("dir open error");	
    exit(EXIT_FAILURE);
  }
  //进入指定的目录 
  ::chdir(dirName.c_str());  //切换工作目录 #include <unistd.h>
 
  //保存对应系统函数返回值的两个变量
  struct dirent * mydirent;  
  struct stat mystat;  
 
  //逐个遍历该目录中的相关项(目录或文件)
  while (NULL != (mydirent = ::readdir(pdir))) {  //读取目录或文件信息 struct dirent* readdir(struct Dir* dirp)
    //获取目录中项的信息
    ::stat(mydirent->d_name, &mystat);  //获取该目录或文件名字 并存于struct stat结构体变量 int stat(const char *path, struct stat *buf) 
    //判断该项是不是目录
    if (S_ISDIR(mystat.st_mode)) { //linux系统函数S_ISDIR(stat.st_mode)判断是否为目录
		
      //如果该目录是'.'和‘..’（每个目录下都有这两项，如果不排除这两项程序会进入无限循环），则跳过该次循环继续下一次
      if (0 == ::strncmp(mydirent->d_name, ".", 1) ||  //int strncmp ( const char * 字符串1, const char * 字符串2, size_t n ) 比较两个字符串前n个相同否
	  0 == ::strncmp(mydirent->d_name, "..", 2)){	
        continue;	
      }
      else {  //如果该目录不是前二者，则递归的遍历目录 
        traverse(mydirent->d_name);	
      }
    }
    else {
      //如果该项不是目录（是文件）,则保存该项的绝对路径
      std::string fileName;
      fileName.append(::getcwd(NULL, 0))             //getcwd()获取当前工作目录
		      .append("/")          
		      .append(mydirent->d_name);     //文件名
      vecFiles_.push_back(fileName);
    }
  }
  ::chdir("..");       //工作目录切换到上级目录
  ::closedir(pdir);    //关闭目录
}

} // end of namespace wd
