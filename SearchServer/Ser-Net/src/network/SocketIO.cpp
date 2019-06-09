#include "SocketIO.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>

namespace wd {

SocketIO::SocketIO(int sockfd)
    : sockfd_(sockfd) {

}


size_t SocketIO::readn(char * buf, size_t count) {  //¿¿ ¿¿¿¿¿¿¿¿ ¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
  size_t nleft = count;
  char * pbuf = buf;
  while (nleft > 0) {   //¿¿¿¿
    int nread = ::read(sockfd_, pbuf, nleft);      //¿¿¿¿¿¿¿,¿¿¿¿¿¿¿¿¿¿¿¿¿
    if (-1 == nread) {        //¿¿¿-1¿¿¿¿
      if (errno == EINTR)     //¿¿¿¿¿¿¿¿¿¿¿¿(¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿)
        continue;
      return EXIT_FAILURE;    //¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
    } else if (0 == nread) {  //¿¿¿0¿¿¿¿¿¿
      break;
    }
    pbuf += nread;   //buf¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿
    nleft -= nread;  //¿¿¿¿¿¿
  }
  return (count - nleft);  //¿¿¿¿¿¿¿¿¿
}

//¿readn()
size_t SocketIO::writen(const char * buf, size_t count) {
  size_t nleft = count;
  const char * pbuf = buf;
  while (nleft > 0) {
    int nwrite = ::write(sockfd_, pbuf, nleft);
    if (nwrite == -1) {
      if(errno == EINTR)
        continue;
	return EXIT_FAILURE;
    }
    nleft -= nwrite;
    pbuf += nwrite;
  }
  return (count - nleft);
}

//¿¿¿¿
size_t SocketIO::recv_peek(char * buf, size_t count) {  //peek¿¿¿¿¿¿¿¿¿¿¿¿¿¿
  int nread;
  do{
    nread = ::recv(sockfd_, buf, count, MSG_PEEK);
  } while (nread == -1 && errno == EINTR);
  return nread;
}

//¿¿¿¿¿¿¿¿¿maxlen - 1¿¿¿¿¿¿¿¿¿¿¿nread¿¿\n¿ ¿¿¿¿¿¿¿¿
//¿¿¿maxlen-1¿¿¿¿¿\n¿¿¿¿¿¿¿\n¿¿¿maxlen - 1¿¿¿¿¿¿¿
size_t SocketIO::readline(char * buf, size_t maxlen) {
  size_t nleft = maxlen - 1;  //¿¿¿¿¿¿¿¿¿¿¿¿¿
  char * pbuf = buf;          //¿¿¿¿¿¿¿¿¿¿
  size_t total = 0;           //¿¿¿¿¿¿¿
  while (nleft > 0) {
    int nread = recv_peek(pbuf, nleft);  //¿¿recv_peek()¿¿¿¿¿¿¿¿¿nread¿¿¿
    if (nread <= 0)
      return nread;

    //¿¿¿¿nread¿¿¿¿¿¿¿\n ¿¿¿¿\n
    for (size_t idx = 0; idx != nread; ++idx) {
      if (pbuf[idx] == '\n') {
        size_t nsize = idx + 1;          // /n¿¿¿+1 ¿\n¿¿¿¿¿¿¿¿
	if (readn(pbuf, nsize) != nsize) //¿¿¿¿¿nsize¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿ 
	  return EXIT_FAILURE;                //¿¿recv_peek()¿¿¿¿¿¿buf¿¿¿¿buf¿¿readn()
	pbuf += nsize;     //¿¿¿¿¿¿
	total += nsize;    //¿¿¿¿¿
	*pbuf = 0;         //¿¿¿¿¿¿¿¿buf¿pbuf=0¿¿¿
	return total;      //¿¿¿¿¿¿¿ ¿¿¿¿¿¿¿¿¿¿¿
      }
    }
    //¿¿¿¿¿¿¿¿¿\n¿¿¿¿¿¿¿¿¿¿¿¿¿buf
    if(readn(pbuf, nread) != nread)
      return EXIT_FAILURE;
    pbuf += nread;
    nleft -= nread;
    total += nread;
  }
  *pbuf = 0;
  return maxlen - 1;  
}

}// end of namespace wd
