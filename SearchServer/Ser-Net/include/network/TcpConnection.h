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

class EpollPoller;         //关联关系 
class TcpConnection;       

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;                        //指针标签 具体连接 
typedef std::function<void(const TcpConnectionPtr &)> TcpConnectionCallback;    //函数标签 回调(作为参数传入回调执行函数)

class TcpConnection : Noncopyable,                                              //基类
	              public std::enable_shared_from_this<TcpConnection> {      //调用智指执行回调时所用辅助类
 public:
  TcpConnection(int sockfd, EpollPoller * loop);   //构造 传入socket文符 和 指向轮寻器类的指针(使之关联关系)
  ~TcpConnection();                                //析构

  std::string receive();                        //接收 封装了Socket::readline() 
  void send(const std::string & msg);           //发送 封装了Socket::readline()
  void sendAndClose(const std::string & msg);   //针对php的发送
  void sendInLoop(const std::string & msg);     //开启事件监听并执行回调的循环
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
  bool isShutdownWrite_;          //本端写状态关闭信号
  EpollPoller * loop_;            //指向轮寻器的指针   
  
  //三个回调在本类里的接收变量
  TcpConnectionCallback onConnectionCb_;
  TcpConnectionCallback onMessageCb_;
  TcpConnectionCallback onCloseCb_;

};

}//end of namespace wd

#endif
