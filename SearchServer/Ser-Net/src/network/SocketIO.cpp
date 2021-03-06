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


size_t SocketIO::readn(char * buf, size_t count) {  //真 真真真真 真真真真真真真�
  size_t nleft = count;
  char * pbuf = buf;
  while (nleft > 0) {   //真真
    int nread = ::read(sockfd_, pbuf, nleft);      //真真真�,真真真真真真�
    if (-1 == nread) {        //真�-1真真
      if (errno == EINTR)     //真真真真真真(真真真真真真真真�)
        continue;
      return EXIT_FAILURE;    //真真真真真真真真�
    } else if (0 == nread) {  //真�0真真真
      break;
    }
    pbuf += nread;   //buf真真真真真真真真�
    nleft -= nread;  //真真真
  }
  return (count - nleft);  //真真真真�
}

//�readn()
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

//真真
size_t SocketIO::recv_peek(char * buf, size_t count) {  //peek真真真真真真真
  int nread;
  do{
    nread = ::recv(sockfd_, buf, count, MSG_PEEK);
  } while (nread == -1 && errno == EINTR);
  return nread;
}

//真真真真�maxlen - 1真真真真真�nread真\n� 真真真真
//真�maxlen-1真真�\n真真真�\n真�maxlen - 1真真真�
size_t SocketIO::readline(char * buf, size_t maxlen) {
  size_t nleft = maxlen - 1;  //真真真真真真�
  char * pbuf = buf;          //真真真真真
  size_t total = 0;           //真真真�
  while (nleft > 0) {
    int nread = recv_peek(pbuf, nleft);  //真recv_peek()真真真真�nread真�
    if (nread <= 0)
      return nread;

    //真真nread真真真�\n 真真\n
    for (size_t idx = 0; idx != nread; ++idx) {
      if (pbuf[idx] == '\n') {
        size_t nsize = idx + 1;          // /n真�+1 �\n真真真真
	if (readn(pbuf, nsize) != nsize) //真真�nsize真真真真真真真真真 
	  return EXIT_FAILURE;                //真recv_peek()真真真buf真真buf真readn()
	pbuf += nsize;     //真真真
	total += nsize;    //真真�
	*pbuf = 0;         //真真真真buf�pbuf=0真�
	return total;      //真真真� 真真真真真�
      }
    }
    //真真真真�\n真真真真真真�buf
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
