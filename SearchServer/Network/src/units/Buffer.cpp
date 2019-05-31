#include "Buffer.h"

namespace wd {
Buffer::Buffer(size_t size)
    : mutex_(),
      notFull_(mutex_),
      notEmpty_(mutex_),
      size_(size),
      flag_(true) {

}

bool Buffer::full() {
  return size_ == que_.size();
}

bool Buffer::empty() {
  return 0 == que_.size();
}
	  
void Buffer::push(Task task) {
  MutexLockGuard guard(mutex_);
  while (full()) {
    notFull_.wait();
  }
  que_.push(task);
  notEmpty_.notify();
}


Buffer::Task Buffer::pop() {
  MutexLockGuard guard(mutex_);
  //mutex_.lock();

  while (empty() && flag_) {
    notEmpty_.wait();
  }

  if (flag_) {
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
