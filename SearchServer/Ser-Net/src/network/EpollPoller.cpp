#include "EpollPoller.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/eventfd.h>


namespace wd {

//创建 epoll实例对象 文符代表(类似socket的文符) 
int createEpollFd() {
  int efd = ::epoll_create1(0);
  if (-1 == efd) {
    perror("epoll_create1 error");
    exit(EXIT_FAILURE);
  }
  return efd;
}

//创建 eventfd实例对象(一种通知机制) 文符代表
int createEventFd() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (-1 == evtfd) {
    perror("eventfd create error");
  }
  return evtfd;
}

// 注册一个新的监听对象sockfd到epoll实例中
void addEpollFdRead(int efd, int fd) {
  struct epoll_event ev;  //设置 struct epoll_event的实例 (也是事件表的类型)
  ev.data.fd = fd;          //绑定与处理事件相关的文符
  ev.events = EPOLLIN;      //绑定要监听的事件类型(有数据传来)
  //对epoll实例(epoll文符)进行操作 操作选项为添加
  int ret = epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev);  //参数(epoll实例, 操作选项, 需要监听的文符, epoll_event的引用或指针)
  if (-1 == ret) {        //0成功-1失败
    perror("epoll_ctl add error");
    exit(EXIT_FAILURE);
  }
}

//删除一个监听对象sockfd从epoll实例中
void delEpollReadFd(int efd, int fd) {
  struct epoll_event ev;
  ev.data.fd = fd;
  int ret = epoll_ctl(efd, EPOLL_CTL_DEL, fd, &ev);  //EPOLL_CTL
  if (-1 == ret) {
    perror("epoll_ctl del error");
    exit(EXIT_FAILURE);
  }
}

//轮询事件循环中对新连接的处理 封装accept()
int acceptConnFd(int listenfd) {
  int peerfd = ::accept(listenfd, NULL, NULL);
  if (peerfd == -1) {
    perror("accept error");
    exit(EXIT_FAILURE);
  }
  return peerfd;
}


//预览数据 私有函数 在SocketIO中也有
size_t recvPeek(int sockfd, void * buf, size_t len) {
  int nread;
  do {
    nread = ::recv(sockfd, buf, len, MSG_PEEK);
  } while (nread == -1 && errno == EINTR);
  return nread;
}

//通过预览数据，判断conn是否关闭 封装recPeek()
bool isConnectionClosed(int sockfd) {
  char buf[1024];
  int nread = recvPeek(sockfd, buf, sizeof(buf));
  if (-1 == nread) {   
    perror("recvPeek--- ");
    return true;  //返回true 关闭
    //exit(EXIT_FAILURE);//若peer端已关闭连接，会导致server端崩溃
  }
  return (0 == nread); //正常nread返回预览的字节数 不为0返回false
}

//==================以上存在于原SocketUtil.h里===============================

//==================以下开始正式的EpollPoller.cpp============================

EpollPoller::EpollPoller(int listenfd)
    : epollfd_(createEpollFd()),           //创epoll得epollfd
      listenfd_(listenfd),                 //传sockfd
      wakeupfd_(createEventFd()),          //创eventfd得eventfd
      isLooping_(false),                   
      eventsList_(1024) {                
  addEpollFdRead(epollfd_, listenfd_);     //添加被监听的sockfd(socket文件描述符)
  addEpollFdRead(epollfd_, wakeupfd_);     //添加被监听的eventfd通知机制(eventfd文件描述符)
}


EpollPoller::~EpollPoller() {
  ::close(epollfd_);
}

void EpollPoller::loop() {
  isLooping_ = true;
  while (isLooping_) {
    waitEpollfd();
  }
}

void EpollPoller::unloop() {
  if(isLooping_)
    isLooping_ = false;
}

//子线程给主线程传递数据过去
void EpollPoller::runInLoop(const Functor & cb) { 
  {
    MutexLockGuard mlg(mutex_);       //封锁住临界缓冲区->压入数据->告知主线程(的epoll对eventfd的监控)
    pendingFunctors_.push_back(cb);
  }
  wakeup();
}

//主线程从缓冲区取出子线程传送过来的数据
void EpollPoller::doPendingFunctors() {
  printf("doPendingFunctors()\n");
  std::vector<Functor> functors;    

  {
    MutexLockGuard mlg(mutex_);       //对缓冲区操作 将交互缓冲区的内容放到本线程中来
    functors.swap(pendingFunctors_);  
  }
	
  for (size_t i = 0; i < functors.size(); ++i) {   //挨个执行其中的回调内容
    functors[i]();
  }
}

//封装对eventfd的write() 计算线程写eventfd通知主线程接收结果 
void EpollPoller::wakeup() {
  uint64_t one = 1;  //8字节长度的数据
  //往wakeupFd_中写入8个字节就可以唤醒一个线程
  ssize_t n = ::write(wakeupfd_, &one, sizeof(one));
  if (n != sizeof(one)) { //返回值不等于8则唤醒失败
    perror("EpollPoller::wakeup() n != 8");
  }
}
//封装对eventfd得read() IO线程读/清空eventfd表示收到通知
void EpollPoller::handleRead() {
  uint64_t one = 1;
  ssize_t n = ::read(wakeupfd_, &one, sizeof(one));
  if (n != sizeof(one)) {
    perror("EpollPoller::handleRead() n != 8");
  }
}

//回调注册函数
void EpollPoller::setConnectionCallback(EpollCallback cb) {
  onConnectionCb_ = cb;
}
void EpollPoller::setMessageCallback(EpollCallback cb) {
  onMessageCb_ = cb;
}
void EpollPoller::setCloseCallback(EpollCallback cb) {
  onCloseCb_ = cb;
}

void EpollPoller::waitEpollfd() {
  //不听地监听
  int nready;
  do {
    nready = ::epoll_wait(epollfd_,                 //监听事件表
			  &(*eventsList_.begin()),
			  eventsList_.size(),
			  5000);
  } while(nready == -1 && errno == EINTR);  //返回-1并中断错误类型是更快的进程导致中断 忽略 继续循环 

  if (nready == -1) {                       //非更快进程导致中断是真正异常
    perror("epoll_wait error");
    exit(EXIT_FAILURE);
  } else if (nready == 0) {                 //超时提醒
    printf("epoll_wait timeout\n");	
  } else {                                  //确实是事件激活  

    //如果事件表激活满了, 做一个扩容
    if (nready == static_cast<int>(eventsList_.size())) {
      eventsList_.resize(eventsList_.size() * 2);
    }
    //遍历每一个激活的文件描述符
    for (int idx = 0; idx != nready; ++idx) {
      if (eventsList_[idx].data.fd == listenfd_) {          //socketfd事件 新连接得标志事件fd是本端sockfd
        if (eventsList_[idx].events & EPOLLIN) {              //EPOLLIN型
	  handleConnection();
	}
      } else if (eventsList_[idx].data.fd == wakeupfd_) {   //eventfd事件
        printf("wakeupfd light\n");
	if (eventsList_[idx].events & EPOLLIN) {              //EPOLLIN型
	  handleRead();                                       //主子线程后向接口中主线程读
	  doPendingFunctors();                                //
	}
      } else {
        if (eventsList_[idx].events & EPOLLIN) {            //其他读事件都是处理信息(包含 处理/关闭)
          handleMessage(eventsList_[idx].data.fd);
        }
      }
    }//end for
  }//end else
}

//建立连接的函数，封装了相应回调
void EpollPoller::handleConnection() {
  int peerfd = acceptConnFd(listenfd_);                        //添加sockfd到epoll
  addEpollFdRead(epollfd_, peerfd);                           

  TcpConnectionPtr conn(new TcpConnection(peerfd, this));      //用对端socketfd创建TcpConnection实例
  //...给客户端发一个欢迎信息==> 挖一个空: 等...
  //conn->send("welcome to server.\n");                     
  conn->setConnectionCallback(onConnectionCb_);                //连接实例注册回调
  conn->setMessageCallback(onMessageCb_);
  conn->setCloseCallback(onCloseCb_);

  std::pair<ConnectionMap::iterator, bool> ret;                //保存对端socket口 和 连接实例对 并检查
  ret = connMap_.insert(std::make_pair(peerfd, conn));
  assert(ret.second == true);
  (void)ret;
  //connMap_[peerfd] = conn;

  conn->handleConnectionCallback();                            //调用连接回调
}

//处理消息的函数，封装了相应回调
void EpollPoller::handleMessage(int peerfd) {
  bool isClosed = isConnectionClosed(peerfd);                  //通过传入该对端的socket口检查对端是否关闭
  ConnectionMap::iterator it = connMap_.find(peerfd);        
  assert(it != connMap_.end());

  if (isClosed) {                                              //对端关闭了调用相应回调并从epoll监控和连接map中删除
    it->second->handleCloseCallback();
    delEpollReadFd(epollfd_, peerfd);
    connMap_.erase(it);
  } else {
    it->second->handleMessageCallback();                       //未关闭则直接调用处理消息的回调
  }
}

}// end of namespace wd
