#ifndef _WD_CONDITION_H
#define _WD_CONDITION_H

#include "Noncopyable.h"
#include <pthread.h>

namespace wd {

class MutexLock;    //私有成员传了该类引用 关联MutexLock 所以加前项申明

class Condition : Noncopyable {
 public:
  Condition(MutexLock & mutex);
  ~Condition();

  void wait();      //三个方法
  void notify();
  void notifyall();

 private:
  pthread_cond_t cond_;    //posix标准库条件变量类对象
  MutexLock & mutex_;      //MutexLock互斥锁对象引用
};

}//end of namespace wd

#endif
