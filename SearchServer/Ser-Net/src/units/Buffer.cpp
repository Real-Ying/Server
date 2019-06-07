#include "Buffer.h"

namespace wd {

Buffer::Buffer(size_t size)      //构造 同时也构造了与之联系的三个对象 (mutexLock对象 Condition对象 Queue对象) 
    : mutex_(),
      notFull_(mutex_),
      notEmpty_(mutex_),
      size_(size),
      flag_(true) {

}

bool Buffer::full() {             //if full, return 1 
  return size_ == que_.size();
}

bool Buffer::empty() {            //if empty, return 1
  return 0 == que_.size();
}
	  
void Buffer::push(Task task) {    //push方法运行在生产者线程
  MutexLockGuard guard(mutex_);   //访问临界资源先加锁，创建保护类对象保护这个锁
  while (full()) {                //不用if用while防止条件变量被异常唤醒
    notFull_.wait();                  //发现是while的情况只能等待
  }
  que_.push(task);                //不在while循环中则满足条件可放入数据，并通知等待条件信号的线程可以取数据了
  notEmpty_.notify();
}


Buffer::Task Buffer::pop() {      //pop方法运行在消费者线程
  MutexLockGuard guard(mutex_);   
  //mutex_.lock();

  while (empty() && flag_) {      //为空并且Buffer不可用则wait循环
    notEmpty_.wait();
  }

  if (flag_) {                    //满足条件则取出返回该数据成员
    Task task = que_.front();
    que_.pop();

    notFull_.notify();
    //mutex_.unlock();
    return task;
  } 
  else {
    //mutex_.unlock();
    return NULL;
  }
}


void Buffer::wakeup_empty() {
  notEmpty_.notifyall();
}

void Buffer::set_flag(bool flag) { 
  flag_ = flag;
}

}//end of namespace wd
