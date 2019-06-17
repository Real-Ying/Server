#include "TcpConnection.h"
#include "EpollPoller.h"
#include <string.h>
#include <stdio.h>

namespace wd {

TcpConnection::TcpConnection(int sockfd, EpollPoller * loop)   //构造 
    : sockfd_(sockfd),                               //sockfd_赋值
      sockIO_(sockfd),                               //构造sockIO的对象
      localAddr_(wd::Socket::getLocalAddr(sockfd)),  //localAddr_赋值 Socket.h常成函
      peerAddr_(wd::Socket::getPeerAddr(sockfd)),    //peerAddr_赋值  Socket.h常成函
      isShutdownWrite_(false),                       //本端写状态信号初始化
      loop_(loop) {                                  //轮询器 将轮询器对象给指针
      
}


TcpConnection::~TcpConnection() {                             //析构
  if (!isShutdownWrite_) {                           //关闭本端写状态
    isShutdownWrite_ = true;
    shutdown();
  }
  printf("~TcpConnection()\n");
}

std::string TcpConnection::receive() {                 //Tcp连接的接收函數，从内核接收缓冲区读一行内容到用户应用空间的buf
  char buf[1024];
  memset(buf, 0, sizeof(buf));  //运用memset()内存中初始化开辟1kb数组空间            
  size_t ret = sockIO_.readline(buf, sizeof(buf));  
  if (ret == 0) {
    return std::string();
  } else
    return std::string(buf);
}

void TcpConnection::send(const std::string & msg) {    //Tcp连接的发送函数 从用户应用空间发送数据到内核发送缓冲区
  sockIO_.writen(msg.c_str(), msg.size());
}

//发送消息后关闭(针对php服务器)
void TcpConnection::sendAndClose(const std::string & msg) {
  send(msg);
  shutdown();
}


void TcpConnection::sendInLoop(const std::string & msg) {
  loop_->runInLoop(std::bind(&TcpConnection::sendAndClose, this, msg));
}

//Tcp连接的关闭
void TcpConnection::shutdown() {   
  if (!isShutdownWrite_)
    sockfd_.shutdownWrite();
  isShutdownWrite_ = true;
}

//socket口以string打印
std::string TcpConnection::toString() {
  char str[100];
  snprintf(str, sizeof(str), "%s:%d -> %s:%d",
	   localAddr_.ip().c_str()
	   localAddr_.port(),
	   peerAddr_.ip().c_str(),
	   peerAddr_.port());
  return std::string(str);
}

//回调注册，在执行函数中被传入使用 
void TcpConnection::setConnectionCallback(TcpConnectionCallback cb) {
  onConnectionCb_ = cb;
}
void TcpConnection::setMessageCallback(TcpConnectionCallback cb) {
  onMessageCb_ = cb;
}
void TcpConnection::setCloseCallback(TcpConnectionCallback cb) {
  onCloseCb_ = cb;
}

//回调执行函数，对传入的回调执行
void TcpConnection::handleConnectionCallback() {
  if (onConnectionCb_)
    onConnectionCb_(shared_from_this());
}
void TcpConnection::handleMessageCallback() {
  if (onMessageCb_)
    onMessageCb_(shared_from_this());
}
void TcpConnection::handleCloseCallback() {
  if (onCloseCb_)
    onCloseCb_(shared_from_this());
}

}// end of namespace wd
