#include "TcpConnection.h"
#include "EpollPoller.h"
#include <string.h>
#include <stdio.h>

namespace wd {

TcpConnection::TcpConnection(int sockfd, EpollPoller * loop)   //构造
    : sockfd_(sockfd), 
      sockIO_(sockfd), 
      localAddr_(wd::Socket::getLocalAddr(sockfd)),  //头文件中包含了Socket.h，利用其成员函数 传入socket文件描述符 获取本端和对端的socket口
      peerAddr_(wd::Socket::getPeerAddr(sockfd)),    
      isShutdownWrite_(false), 
      loop_(loop) {
      
}


TcpConnection::~TcpConnection() {                             //析构
  if (!isShutdownWrite_) {                          //查看本端写状态，若未关闭则关闭	  
    isShutdownWrite_ = true;
    shutdown();
  }
  printf("~TcpConnection()\n");
}

std::string TcpConnection::receive() {     //Tcp连接的接收函數，从内核接收缓冲区读一行内容到用户应用空间的buf
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

//针对php服务器
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

//string形式打印socket口
std::string TcpConnection::toString() {
  char str[100];
  snprintf(str, sizeof(str), "%s:%d -> %s:%d",
	   localAddr_.ip().c_str()
	   localAddr_.port(),
	   peerAddr_.ip().c_str(),
	   peerAddr_.port());
  return std::string(str);
}


void TcpConnection::setConnectionCallback(TcpConnectionCallback cb) {
  onConnectionCb_ = cb;
}

void TcpConnection::setMessageCallback(TcpConnectionCallback cb) {
  onMessageCb_ = cb;
}

void TcpConnection::setCloseCallback(TcpConnectionCallback cb) {
  onCloseCb_ = cb;
}

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
