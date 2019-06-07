
#ifndef _NONCOPYABLE_H
#define _NONCOPYABLE_H

namespace wd {

class Noncopyable {
 protected:
  Noncopyable() {}
  ~Noncopyable() {}

  Noncopyable(const Noncopyable & rhs) = delete;          //构造参数为自己时设为delete无法构造和=赋值
  Noncopyable & operator=(const Noncopyable & rhs) = delete;
};

}//end of namespace wd
#endif
