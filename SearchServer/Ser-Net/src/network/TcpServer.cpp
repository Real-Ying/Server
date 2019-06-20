#include "TcpServer.h"

namespace wd {

TcpServer::TcpServer(unsigned short port)                       /***1***/
    : inetAddr_(port),
      sockfd_(),
      epollfd_(sockfd_.fd()) {
  sockfd_.ready(inetAddr_);
}


TcpServer::TcpServer(const char * pIp, unsigned short port)
    : inetAddr_(pIp, port),
      sockfd_(),
      epollfd_(sockfd_.fd()) {
  sockfd_.ready(inetAddr_);
}

void TcpServer::start() {                                       /***2***/
  epollfd_.loop();
}

void TcpServer::stop() {
  epollfd_.unloop();
}

void TcpServer::setConnectionCallback(TcpServerCallback cb) {   /***3***/
  epollfd_.setConnectionCallback(cb);
}

void TcpServer::setMessageCallback(TcpServerCallback cb) {
  epollfd_.setMessageCallback(cb);
}

void TcpServer::setCloseCallback(TcpServerCallback cb) {
  epollfd_.setCloseCallback(cb);
}

}//end of namespace wd
