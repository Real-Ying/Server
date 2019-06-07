#include "InetAddress.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

namespace wd {
//传入端口号构造
InetAddress::InetAddress(short port) {
  ::memset(&addr_, 0, sizeof(addr_));    //为struct sockaddr_in结构体数据开辟数组内存空间并初始化0
  addr_.sin_family = AF_INET;            //协议族使用TCP/ipv4
  addr_.sin_port = htons(port);          //htons()大小端模式转换
  addr_.sin_addr.s_addr = INADDR_ANY;    //本端ip地址
}

//传入ip地址和端口号构造
InetAddress::InetAddress(const char * pIp, short port) {
  ::memset(&addr_, 0, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_port = htons(port);                     //htons()主机字节序转为网路字节序(大小端转换)
  addr_.sin_addr.s_addr = inet_addr(pIp);           //inet_addr()将点分十进制ip转为网络ip地址格式
}

//直接传入struct socket_in 网络通信地址构造
InetAddress::InetAddress(const struct sockaddr_in & addr)
    : addr_(addr){
    
}

//返回struct sockaddr_in 对象的地址
const struct sockaddr_in * InetAddress::getSockAddrPtr() const {
 
  return &addr_;
}

std::string InetAddress::ip() const {        
  
  return std::string(inet_ntoa(addr_.sin_addr));    //inet_ntoa()将网络ip地址格式转化为点分十进制ip
}

unsigned short InetAddress::port() const {

  return ntohs(addr_.sin_port);                     //ntoh()网络字节序转为主机字节序
}


}// end of namespace wd

