
#include "GlobalDefine.hpp"
#include "Configuration.hpp"
#include "DirScanner.hpp"
#include "PageLib.hpp"
#include "WordSegmentation.hpp"
#include "WebPage.hpp"
#include "PageLibPreprocessor.hpp"

#include <stdio.h>
#include <time.h>

int main(void) {
  wd::Configuration conf("conf/my.conf");  //传入同目录下的配置文件目录，将各项配置放入map容器
  conf.debug();                            //打印配置map容器中各项的路径

  wd::DirScanner dirScanner(conf);         //传入语料文件目录，将语料文件路径放入vec容器
  dirScanner();                            
  dirScanner.debug();                      //打印容器中各个语料文件路径

  wd::PageLib pagelib(conf, dirScanner);   //传入配置和语料文件构造PageLib对象
  
  time_t t1 = time(NULL);                 //通过PageLib类函数将这些语料文件处理为 网页文章库和网页位置偏移库 并分别存于map和vec

  pagelib.create();                        
  pagelib.store();
  time_t t2 = time(NULL);
  printf("raw lib create and store time: %d min\n", (t2 - t1));  //打印处理时间

  wd::PageLibPreprocessor libPreprocessor(conf);   //创建网页预处理类对象
  libPreprocessor.doProcess();                     //重建网页库和网页位置偏移库及倒向索引

  return 0;
}
