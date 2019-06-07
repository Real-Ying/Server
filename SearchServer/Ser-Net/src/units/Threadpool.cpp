#include "Threadpool.h"
#include "Thread.h"
#include <stdio.h>

namespace wd {
Threadpool::Threadpool(size_t threadsNum, size_t bufNum)  //线程池构造 传入两个容器的大小
    : threadsNum_(threadsNum),
      buf_(bufNum),
      isExit_(false) {
  printf("Threadpool()\n");
}

Threadpool::~Threadpool() {                               //线程池析构 直接用stop()
  if (!isExit_) {
    stop();
  }
}

void Threadpool::start() {                                                       //创建 并开启运行线程池
  //循环创建单个线程(派生类型)压入线程池容器
  for (size_t idx = 0; idx != threadsNum_; ++idx) {
    Thread * pThread = new Thread(std::bind(&Threadpool::threadFunc, this));
    vecThreads_.push_back(pThread);
  }
  //创建迭代器逐个开启线程
  std::vector<Thread *>::iterator it;
  for (it = vecThreads_.begin(); it != vecThreads_.end(); ++it) {
    (*it)->start();
  }
  printf("Threadpool::start()\n");
}


void Threadpool::stop() {
  //在线程池运行状态下,两个容器退出状态量设置后 循环关闭线程池中的线程
  if (!isExit_) {
    isExit_ = true;
    buf_.set_flag(false);
    buf_.wakeup_empty();

    std::vector<Thread *>::iterator it;
    for (it = vecThreads_.begin(); it != vecThreads_.end(); ++it) {
      (*it)->join();
      delete (*it);           //释放迭代器
    }
    vecThreads_.clear();   //释放容器
  }
}

//TaskQueue的生产者接口
void Threadpool::addTask(Task task) {
  printf("Threadpool::addTask()\n");
  buf_.push(task);
}

//TaskQueue的消费者接口
Threadpool::Task Threadpool::getTask() {
  return buf_.pop();
}

//消费者调用 线程本身的工作步骤 执行
void Threadpool::threadFunc() {
  while (!isExit_) {
    Task task = buf_.pop();
    if (task) {
      task();
    }
  }
}


}// end of namespace wd
