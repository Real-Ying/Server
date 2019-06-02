#ifndef _WD_MUTEXLOCK_H
#define _WD_MUTEXLOCK_H

#include "Noncopyable.h"
#include <pthread.h>

namespace wd {

class MutexLock : Noncopyable {
 public:
  MutexLock();                          //构造
  ~MutexLock();                         //析构

  void lock();                          //加锁方法
  void unlock();                        //解锁方法
	
  pthread_mutex_t * getMutexLockPtr();  //获取标准锁对象的常量成员函数


 private:
  pthread_mutex_t mutex_;               //标准锁对象
  bool isLocked_;                       //锁状态信号
};


class MutexLockGuard {  //锁的保护类，构造时加锁 析构时解锁
 public:
  MutexLockGuard(MutexLock & mutex);
  ~MutexLockGuard();

 private:
  MutexLock & mutex_;   //MutexLock对象引用
};

}// end of namespace wd

#endif
