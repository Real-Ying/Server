#include "MutexLock.h"

namespace wd {

MutexLock::MutexLock()                               //构造 传入锁的初始状态信号，pthread.h标准库函数初始化锁
    : isLocked_(false) {
  pthread_mutex_init(&mutex_, NULL);
}

MutexLock::~MutexLock() {                            //析构 pthread.h标准库方法销毁锁
  if (!isLocked_)
    pthread_mutex_destroy(&mutex_);
}

void MutexLock::lock() {                             //加锁 实际上是把系统提供的API封装一层 设置状态
  pthread_mutex_lock(&mutex_);
  isLocked_ = true;
}

void MutexLock::unlock() {                           //解锁 同
  pthread_mutex_unlock(&mutex_);
  isLocked_ = false;
}

pthread_mutex_t * MutexLock::getMutexLockPtr() {     //常量成员函数获取 锁对象 的引用
  return &mutex_;
}

MutexLockGuard::MutexLockGuard(MutexLock & mutex)    //传入MutexLock对象引用，再封装一层实现了互斥锁保护
    : mutex_(mutex) {
  mutex_.lock();
}

MutexLockGuard::~MutexLockGuard() {
  mutex_.unlock();
}

}//end of namespace wd
