
#include "TcpServer.h"
#include "WordQuery.hpp"
#include "Threadpool.h"

#include <stdio.h>

#include <string>
#include <functional>


using namespace std;
using namespace wd;

class WordQueryServer {
 public:
  WordQueryServer(const string & configfile);  //构造函数 传入配置文件

  void start();                                //启动函数

 private:
  //三个回调
  void onConnection(const TcpConnectionPtr & conn);    
  void onMessage(const TcpConnectionPtr & conn);
  void onClose(const TcpConnectionPtr & conn);
  
  //业务
  void doTaskThread(const TcpConnectionPtr & conn, const string & msg);

 private:
  Configuration _conf;            //配置类对象
  WordQuery _wordQuery;           //查询类对象
  TcpServer _tcpServer;           //网络类对象
  Threadpool _pool;               //线程池对象
};

//构造函数
WordQueryServer::WordQueryServer(const string & configfile)
    : _conf(configfile),          //传入配置文件名/路径      构造配置类对象
      _wordQuery(_conf),          //传入配置类对象           构造查询类
      _tcpServer(5080),           //传入端口号               构造网络类对象(IO主线程)
      _pool(4, 10) {              //传入任务队列和线程池大小 构造线程池类对象(计算子线程)
  
  //网路类中注册三个回调
  _tcpServer.setConnectionCallback(std::bind(&WordQueryServer::onConnection, this, placeholders::_1));
  _tcpServer.setMessageCallback(std::bind(&WordQueryServer::onMessage, this, placeholders::_1));
  _tcpServer.setCloseCallback(std::bind(&WordQueryServer::onClose, this, placeholders::_1));

}

//启动计算线程和IO线程
void WordQueryServer::start() {
  _pool.start();
  _tcpServer.start();
}

//新连接回调
void WordQueryServer::onConnection(const TcpConnectionPtr & conn) {
  printf("%s\n", conn->toString().c_str());
}

//旧连接数据回调
void WordQueryServer::onMessage(const TcpConnectionPtr & conn) {
  string msg(conn->receive());
  size_t pos = msg.find('\n');
  msg = msg.substr(0, pos);
  cout << "client:" << msg << ",size:" << msg.size() << endl;

  //string ret = _wordQuery.doQuery(msg);
  //cout << "result's size:" << ret.size() << endl;
  //conn->send(ret);

  _pool.addTask(std::bind(&WordQueryServer::doTaskThread, this, conn, msg));  //压入任务队列
}

//连接关闭回调
void WordQueryServer::onClose(const TcpConnectionPtr & conn) {
  printf("%s close.\n", conn->toString().c_str());
}

//具体业务
void WordQueryServer::doTaskThread(const TcpConnectionPtr & conn, const string & msg) {
  string ret = _wordQuery.doQuery(msg);

  int sz = ret.size();
  printf("result's size:%d\n",sz); 
  //printf("%s\n\n", ret.c_str());
  conn->sendInLoop(ret);    //计算完异步方式给IO线程
}

int main(void) {
  WordQueryServer wordQueryServer("./conf/my.conf");    //传入配置定义对象
  wordQueryServer.start();                              //启动
	
  return 0;

}
