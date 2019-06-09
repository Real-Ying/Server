#ifndef _WD_SOCKETIO_H
#define _WD_SOCKETIO_H
#include <stdio.h>

namespace wd {

class SocketIO {
 public:
  SocketIO(int sockfd);
  //接收/发送文件时确认收发到count位才完成
  size_t readn(char * buf, size_t count);         //封装::read(3)  同时解决了其粘包问题   
  size_t writen(const char * buf, size_t count);  //封装::write(3) 解决粘包
  //读取以/n结尾的一行
  size_t readline(char * buf, size_t max_len);    //由recv_peek()和readn()实现

 private:
  //预读取
  size_t recv_peek(char * buf, size_t count);     //封装recv()的第四个参数不为0的第三种预取模式

 private:
  int sockfd_;
};


}// end of namespace wd


#endif

