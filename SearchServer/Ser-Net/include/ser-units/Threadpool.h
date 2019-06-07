#ifndef __WD_THREADPOOL_H
#define __WD_THREADPOOL_H

#include "Buffer.h"
#include <vector>
#include <functional>

namespace wd {

class Thread;  //Threadpool的基类 前置申明

class Threadpool {
 public:
  typedef std::function<void()> Task;
  Threadpool(size_t threadsNum, size_t bufNum);
  ~Threadpool();

  void start();
  void stop();

  void addTask(Task task);                         //添加任务 外部生产者向任务队列添加任务的外部接口

 private:
  void threadFunc();                 //调用任务执行步骤的包裹函数
  Task getTask();                    //提取任务 内部消费者从任务队列提取任务的内部接口

 private:
  size_t threadsNum_;                   //线程池及其大小
  std::vector<Thread *> vecThreads_;
  Buffer buf_;                          //任务队列
  bool isExit_;                         //线程池退出信号
};

}// end of namespace wd

#endif
