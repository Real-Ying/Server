#include "Socket.h"
#include "InetAddress.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace wd {

Socket::Socket(int sockfd) : sockfd_(sockfd) {

}

Socket::Socket() {
  sockfd_ = socket(AF_INET, SOCK_STREAM, 0);   //创建一个tcp/ipsocket文件描述符
  if (-1 == sockfd_) {
    perror("socket error");
  }
}

Socket::~Socket() {
  ::close(sockfd_);                            //系统函数关闭文件描述符读写
}

//将四个准备工作设为私有放进用公有的ready()方法一起调用
void Socket::ready(const InetAddress & addr) {
  setReuseAddr(true);                          
  setReusePort(true);
  bindAddress(addr);
  listen();
}


void Socket::bindAddress(const InetAddress & addr) {
  if (-1 == ::bind(sockfd_, (const struct sockaddr*)addr.getSockAddrPtr(), sizeof(InetAddress))) {
    perror("bind error");
    ::close(sockfd_);
    exit(EXIT_FAILURE);
  }
}


void Socket::listen() {
  if(-1 == ::listen(sockfd_, 10)) {  //第二个参数的意义是 放置 处在第一次发送syn到被accept这个时间区间中暂未完成连接请求的数量 的队列最大长度
    perror("listen error");
    ::close(sockfd_);
    exit(EXIT_FAILURE);              //EXIT_FAILURE宏定义为1，表示没有成功运行程序，可作为exit()的参数以退出
  }
}

void Socket::setReuseAddr(bool flag) {
  int on = (flag ? 1 : 0);
  if (::setsockopt(sockfd_,          //文件描述符
	           SOL_SOCKET,       //层次的选项(套接字级别)
		   SO_REUSEADDR,     //设置的选项(地址复用功能)
		   &on,              //指向存放选项值空间的引用/地址
		   static_cast<socklen_t>(sizeof(on))) == -1) {  //选项值空间的长度 成功返0失败返-1
    perror("setsockopt reuseaddr error");
    ::close(sockfd_);
    exit(EXIT_FAILURE);
  }
}

void Socket::setReusePort(bool flag) {
#ifdef SO_REUSEPORT
  int on = (flag ? 1 : 0);
  if (::setsockopt(sockfd_, 
		  SOL_SOCKET,
		  SO_REUSEPORT,       //设置的选项(端口复用功能)
		  &on,
		  static_cast<socklen_t>(sizeof(on))) == -1) {
    perror("setsockopt reuseport error");
    ::close(sockfd_);
    exit(EXIT_FAILURE);
  }
#else
  if (flag) {
    fprintf(stderr, "SO_REUSEPORT is not supported!\n");
  }
#endif
}

int Socket::accept() {
  int peerfd = ::accept(sockfd_, NULL, NULL);	
  if (-1 == peerfd) {
    perror("accept error");
    ::close(sockfd_);
  }
  return peerfd;
}

void Socket::shutdownWrite() {
  if (::shutdown(sockfd_, SHUT_WR) == -1) {
    perror("shutdown write error");
    //exit(EXIT_FAILURE);//若peer端已关闭，会导致server端崩溃
  }
}

InetAddress Socket::getLocalAddr(int sockfd) {
  struct sockaddr_in addr;
  socklen_t len = sizeof(sockaddr_in);
  if (::getsockname(sockfd, (struct sockaddr *)&addr, &len) == -1) {
    perror("getsockname error");
  }
  return InetAddress(addr);
}

InetAddress Socket::getPeerAddr(int sockfd) {
  struct sockaddr_in addr;
  socklen_t len = sizeof(sockaddr_in);
  if (::getpeername(sockfd, (struct sockaddr *)&addr, &len) == -1) {
    perror("getpeername error");
  }
  return InetAddress(addr);
}

}// end of namespace wd
