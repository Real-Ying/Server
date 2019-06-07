#ifndef _MY_THREAD_H_
#define _MY_THREAD_H_

#include "Noncopyable.h"
#include <pthread.h>                                 //posix标准线程头文件
#include <functional>

namespace wd {

class Thread : Noncopyable {
 public:
  typedef std::function<void()> ThreadCallback;      //简化回函类型定义
  Thread(ThreadCallback cb);                         //构造函数，传入 回函...
  ~Thread();                                         //析构

  void start();                                      //线程启动
  void join();                                       //线程关闭

  static void * threadFunc(void * arg);              //pthread_create()所需的类型和参数都是void型的第三个参数，由于成员函数第一参数固有this 
                                                     //故而函数设为静态去掉this
 private:
  pthread_t pthId_;                                  //线程id，posix标准线程必有
  bool isRunning_;                                   //线程状态量
  ThreadCallback cb_;                                //回函对象
};

}//end of namespace wd
#endif
