#ifndef _WD_TCPCONNCETION_H
#define _WD_TCPCONNCETION_H

#include "Noncopyable.h"
#include "InetAddress.h"
#include "Socket.h"
#include "SocketIO.h"

#include <string>
#include <memory>
#include <functional>

namespace wd {

class EpollPoller;   
class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;                        //标签  

class TcpConnection : Noncopyable,                                              
	              public std::enable_shared_from_this<TcpConnection> {      //执行回调所用辅助类
 public:
  typedef std::function<void(const TcpConnectionPtr &)> TcpConnectionCallback;  //本类中回调的类型(作为执行回调函数的参数)
  TcpConnection(int sockfd, EpollPoller * loop);   //构造 传入socket文符 和 EpollPoller类的指针(使之关联关系)
  ~TcpConnection();                                //析构

  std::string receive();                        //接收 封装了Socket::readline() 
  void send(const std::string & msg);           //发送 封装了Socket::readline()
  void sendAndClose(const std::string & msg);   //针对php的发送
  void sendInLoop(const std::string & msg);     //循环发送
  void shutdown();                              //关闭连接(写功能) 封装了Socket::shutdownwrite()

  std::string toString();                       //将socket口string格式形式打印
  
  //三个回调在本类里的注册赋值函数
  void setConnectionCallback(TcpConnectionCallback cb);  
  void setMessageCallback(TcpConnectionCallback cb);
  void setCloseCallback(TcpConnectionCallback cb);
  
  //三个回调的执行函数
  void handleConnectionCallback();
  void handleMessageCallback();
  void handleCloseCallback();

 private:
  Socket sockfd_;                 //socket对象 
  SocketIO sockIO_;               //SocketIO类对象 包含了TcpConection读写的实现
  const InetAddress localAddr_;   //本端socket口地址
  const InetAddress peerAddr_;    //对端socket口地址
  bool isShutdownWrite_;          //本端写状态变量
  EpollPoller * loop_;            //EpollPoller指针对象   
  
  //三个回调在本类里的接收变量
  TcpConnectionCallback onConnectionCb_;
  TcpConnectionCallback onMessageCb_;
  TcpConnectionCallback onCloseCb_;

};

}//end of namespace wd

#endif
