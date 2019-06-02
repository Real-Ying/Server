#ifndef _WD_BUFFER_H
#define _WD_BUFFER_H 

#include "Noncopyable.h"
#include "MutexLock.h"
#include "Condition.h"
#include <queue>
#include <functional>

namespace wd {

class Buffer : Noncopyable {
 public:
  typedef std::function<void()> Task;  //由于Task要绑定线程 所以设为void
  Buffer(size_t size);                 //构造 由于它的数据成员都能自己析构 所以可无析构

  void push(Task task);                //对外的两个操作接口
  Task pop();

  bool full();                         //两个状态判断函数
  bool empty();

  void wakeup_empty();
  void set_flag(bool flag);

 private:
  MutexLock          mutex_;      //互斥锁对象

  Condition          notFull_;    //两个状态信号量
  Condition          notEmpty_;

  size_t             size_;       //Queue' size
  std::queue<Task>   que_;        //Queue对象

  bool               flag_;       //Buffer的状态信号量
};

}// end of namespace wd

#endif
