#ifndef __WD_SOCKET_H
#define __WD_SOCKET_H
//class Socket & class Accept
#include "Noncopyable.h"

//每当有文件描述符生成都交给socket类来控制开启关闭并可获得两端网络地址
//文件描述符由socket()返回

namespace wd {
class InetAddress;
class Socket : Noncopyable {    //基础类禁止系统默认复制
 public:
  Socket(int sockfd);   //传入已存在的文件描述符构造
  Socket();             //无参构造(参数内部创建文件描述符)
  ~Socket();            //析构 关闭该文件描述符的双工连接

  void ready(const InetAddress & addr);    //封装四个做准备的方法

  int accept();                            //封装::accept() 接受对端连接请求
  void shutdownWrite();                    //封装::shutdown()设置 关闭本端写功能
  int fd() { return sockfd_; }                   //获取文件描述符

  static InetAddress getLocalAddr(int sockfd);   //封装::getsockname()和::getpeername() 传入文件描述符获取本端/对端的socket地址
  static InetAddress getPeerAddr(int sockfd);    
 
 //ready()中的四个准备方法
 private:
  void setReuseAddr(bool flag);                     //封装::setsockopt()设置地址和端口是否复用
  void setReusePort(bool flag);                     
  void bindAddress(const InetAddress & addr);       //封装::bind() 将socket文件描述符与socket口绑定
  void listen();                                    //封装::listen() 
 private:
  int sockfd_;         //socket文件描述符
};
}// end of namespace wd

#endif
