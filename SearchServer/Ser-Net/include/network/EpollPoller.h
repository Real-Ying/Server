#ifndef __WD_EPOLLPOLLER_H
#define __WD_EPOLLPOLLER_H

#include "Noncopyable.h"
#include "TcpConnection.h"
#include "MutexLock.h"
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <functional>

namespace wd {
class EpollPoller : Noncopyable {
 public:
  typedef TcpConnection::TcpConnectionCallback EpollCallback; //函数标签 回调
  typedef std::function<void()> Functor;                      //函数标签 空参void型

  EpollPoller(int listenfd);   //构造 传入文件描述符                           
  ~EpollPoller();              //析构

  void loop();                          //轮询事件循环 封装waitEpollfd()
  void unloop();                        //停止loop()
  void runInLoop(const Functor & cb);   //计算线程将结果作为函数传递给和IO线程(EpollPoller对象loop)之间的缓冲
  void doPendingFunctors();             //IO线程 取出所有缓冲中的数据回调并挨个执行

  void wakeup();         //封装计算线程对eventfd的write()，以通知IO线程
  void handleRead();     //封装 IO 线程对eventfd的read()表示收到并重启该内核计数器
  
  //回调在本类中的注册 回调在test_main()中实现并传递过来
  void setConnectionCallback(EpollCallback cb);
  void setMessageCallback(EpollCallback cb);
  void setCloseCallback(EpollCallback cb);

 private:
  void waitEpollfd();               //封装handleConnection() 和 handleMessage()
  void handleConnection();          //处理新连接
  void handleMessage(int peerfd);   //处理已有连接发送过来的消息
	
 private:
  int epollfd_;                                              //epoll 文符 代表轮询器 
  int listenfd_;                                             //sock  文符 用于管理保存连接map<sockfd, TcpConnection>.first 
  int wakeupfd_;                                             //eventfd
  bool isLooping_;                                           //loop()的退出信号

  MutexLock mutex_;                                          //互斥锁
  std::vector<Functor> pendingFunctors_;                     //容器 后向接口上计算线程传递给IO线程的数据(回调形式)的缓冲

  typedef std::vector<struct epoll_event> EventList;         //容器标签 事件表
  EventList eventsList_;                                     //容器 事件表
  typedef std::map<int, TcpConnectionPtr> ConnectionMap;     //容器标签 存储连接实例的map
  ConnectionMap connMap_;                                    //容器 保存<sockfd,Tcp实例指针>的map

  //三个回调在本类中的暂存变量
  EpollCallback onConnectionCb_;
  EpollCallback onMessageCb_;
  EpollCallback onCloseCb_;
};

}//end of namespace wd

#endif
