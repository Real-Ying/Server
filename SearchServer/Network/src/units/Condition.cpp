#include "Condition.h"
#include "MutexLock.h"


namespace wd {

Condition::Condition(MutexLock & mutex)     //构造 传入锁对象 标准库函数init    
    : mutex_(mutex) {
  pthread_cond_init(&cond_, NULL);
}

Condition::~Condition() {                   //析构 传入条件变量对象引用 标准库函数销毁
  pthread_cond_destroy(&cond_);
}

void Condition::wait() {                    //标准库APIwait 注意参数
  pthread_cond_wait(&cond_, mutex_.getMutexLockPtr());
}

void Condition::notify() {                  //标准库notify
  pthread_cond_signal(&cond_);
}

void Condition::notifyall() {               //标准库notifyall
  pthread_cond_broadcast(&cond_);
}


}//end of namespace wd
