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

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

class TcpConnection : Noncopyable,
	              public std::enable_shared_from_this<TcpConnection> {
 public:
  typedef std::function<void(const TcpConnectionPtr &)> TcpConnectionCallback;
  TcpConnection(int sockfd, EpollPoller * loop);
  ~TcpConnection();

  std::string receive();                        //接收 封装了Socket::readline() 
  void send(const std::string & msg);           //发送 封装了Socket::readline()
  void sendAndClose(const std::string & msg);
  void sendInLoop(const std::string & msg);
  void shutdown();                              //关闭连接(写功能) 封装了Socket::shutdownwrite()

  std::string toString();

  void setConnectionCallback(TcpConnectionCallback cb);
  void setMessageCallback(TcpConnectionCallback cb);
  void setCloseCallback(TcpConnectionCallback cb);

  void handleConnectionCallback();
  void handleMessageCallback();
  void handleCloseCallback();

 private:
  Socket sockfd_;                 //socket文件描述符
  SocketIO sockIO_;               //SocketIO类对象 包含了TcpConection读写的实现
  const InetAddress localAddr_;   //本端socket口地址
  const InetAddress peerAddr_;    //对端socket口地址
  bool isShutdownWrite_;          //本端写状态变量
  EpollPoller * loop_;

  TcpConnectionCallback onConnectionCb_;
  TcpConnectionCallback onMessageCb_;
  TcpConnectionCallback onCloseCb_;

};

}//end of namespace wd

#endif
