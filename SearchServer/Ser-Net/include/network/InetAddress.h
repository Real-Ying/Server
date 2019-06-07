#ifndef _WD_INETADDRESS_H
#define _WD_INETADDRESS_H


#include <netinet/in.h>
#include <string>

namespace wd {

class InetAddress {
 public:
  //三种构造方式
  InetAddress(short port);
  InetAddress(const char * pIp, short port);
  InetAddress(const struct sockaddr_in & addr);

  const struct sockaddr_in * getSockAddrPtr() const;  //获取struct sockaddr_in结构体数据的指针(bind()参数所需)
  
  std::string ip() const;
  unsigned short port() const;

 private:
  struct sockaddr_in addr_;   //网络通讯地址数据类对象  struct sockaddr_in是linux内核针对ipv4用到的socket address结构体
};


}// end of namespace wd

#endif
