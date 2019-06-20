#ifndef _WD_TCPSERVER_H
#define _WD_TCPSERVER_H

#include "InetAddress.h"
#include "Socket.h"
#include "EpollPoller.h"

namespace wd {

class TcpServer {
 public:
  //回调函数标签，在每个有注册的类中都有 标签名不同 类型相同
  typedef TcpConnection::TcpConnectionCallback TcpServerCallback;  

  TcpServer(unsigned short port);                      //构造 两种
  TcpServer(const char * pIp, unsigned short port);

  void start();                                        //启动 和 关闭
  void stop();

  void setConnectionCallback(TcpServerCallback cb);    //三个注册
  void setMessageCallback(TcpServerCallback cb);
  void setCloseCallback(TcpServerCallback cb);

 private:
  InetAddress inetAddr_;                               //socket口对象
  Socket sockfd_;                                      //socket对象
  EpollPoller epollfd_;                                //Epollpoller对象

  TcpServerCallback onConnectionCb_;                   //三个回调暂存
  TcpServerCallback onMessageCb_;
  TcpServerCallback onCloseCb_;
};

}// end of namespace wd

#endif
