
#include "Thread.h"
#include <iostream>

namespace wd {

Thread::Thread(ThreadCallback cb)                   //构造函数，传入 回函 线程id 线程状态量   
    : pthId_(0),
      isRunning_(false),
      cb_(cb) {
  std::cout << "Thread()" << std::endl;
}

void Thread::start() {  
  pthread_create(&pthId_, NULL, threadFunc, this);  //pthread库方法创建线程，参数 1.线程id 2.线程属性可设NULL 3.业务函数指针 4.指向该新建线程的指针
  isRunning_ = true;
}

void Thread::join() {                               //pthread库方法等待线程结束将其关闭(若正在运行)
  if (isRunning_) {
    pthread_join(pthId_, NULL);
    isRunning_ = false;
  }
}

void* Thread::threadFunc(void * arg) {              //线程业务函数(包裹回函)，传入pthread_create()中新建地址的指针this，使得业务功能 与 线程 绑定
  Thread * pThread = static_cast<Thread *>(arg);        //由于新生成的this(形参arg)都是void*型，需转化为Thread*型
  if (pThread != NULL) {	                        //业务函数里必有回函，执行回函 并 返回该线程的指针
    pThread->cb_();
    return pThread;
  }
  return NULL;	
}

Thread::~Thread() {                                 //析构 jion()没有或不起作用(被阻塞)的情况下回收线程资源
  if (isRunning_)
    pthread_detach(pthId_);
    std::cout << "~Thread()" << std::endl;
}

} // end of namespace wd
