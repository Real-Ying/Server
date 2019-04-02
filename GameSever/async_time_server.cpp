#include<ctime>

#include<iostream>
#include<string>
#include<memory>    //存放与内存相关的头文件 比如智能指针
#include<functional>    //存放与函数相关的头文件 比如lambda

#include<boost/asio.hpp>

using boost::asio::ip::tcp;    //为了编写方便不用写一长串的namespace

std::string make_daytime_string(){
    using namespace std;    //为了time和ctime
    auto now = time(nullptr);
    return ctime(&now);
}

//同步是阻塞式的每次只处理一个连接，但异步可能同时要处理多个连接，那么就需要用一个连接类和他的实例来区分出来，类中存储每一个连接的信息
class tcp_connection : public std::enable_shared_from_this<tcp_connection>{    
  public:
    using pointer = std::shared_ptr<tcp_connection>;  //简化代码
    
    //使用静态的类函数传入一个io_service引用生成一个用智能指针包裹的实例
    static pointer create(boost::asio::io_service &io_service){
       // return std::make_shared<tcp_connection>(io_service);	//std::make_shared 功能传入一个参数返回一个某种类型的智能指针
       return pointer(new tcp_connection(io_service));  //虽然此种创建要生成两次指针，但不用调用构造函数这里适用
    }
    
    tcp::socket &socket() { return socket_; }  //用引用方式返回成员数据

    // 
    void start(){
        message_ = make_daytime_string();  //返回当前时间存到数据成员里
        
	//asio里异步方式给buffer写内容message发送给socket_即客户端(异步地写不管成败都会立即返回)，写完之后恰当时机调用lambda回调函数
	auto self = shared_from_this(); //这个智能指针用来包裹lambda
	boost::asio::async_write(
	    socket_, boost::asio::buffer(message_),
	    [self = std::move(self)](auto error, auto bytes_transferred){  //这里用move替代直接[self]值拷贝 因为上面已经生成了一份再拷贝一份影响效率
	        self->handle_write(error, bytes_transferred);  //这是bind(tcp_connection::handle_write,self,参1,参2)修改的 即智能this调用handle_write
	    }                                                  //bind里面绑定的东西的生命周期是和返回的boost function的生命周期一样，而后者是不会失效的
			)

    }

  private：
    tcp_connection(boost::asio::io_service &io_service) : socket_(io_service){}  //构造函数在private里，上面std::make_shared<>会调用构造函数所以得换方法
    void handle_write(boost::system::error_code &error, size_t  bytes_transferrred){}  //这是回调函数实际上没有做什么
    tcp::socket socket_;
    std::string message_;

}
/******************构造时绑定io_service ip 并且等待新连接，这个等待新连接里首先连接第一个把new_connection构造出来，连接成功或失败不管这么着 如果成功了就调用start()开始干活，之后连接新的客户端************************/
class tcp_server{
  public:
    tcp_server(boost::asio::io_service &io_service)    //构造函数 传入一个io_service 绑定ip和端口号 为建立监听连接做准备
        :acceptor(io_service, tcp::endpoint(tcp::v4), 13){
        start_accept();    //建立一个监听连接
    }	         
  private:
    void start_accept){
	auto new_connection = tcp_connection::creat(acceptor.get_io_service); //acceptor类中的成员来得到对应io_service的引用

	acceptor.async_accept(new_connection->socket(), 
	    [new_connection,this](auto error){
	        this->handle_accept(new_connection,error);
	    });  //acceptor以异步的方式建立一个客户端监听连接 这个连接需要一个对应的socket 和 一个回调函数(因为connection用了指针包裹所以要有一个this)
    } 

    void handle_accept(tcp_connection::pointer new_connection,  //第一个参数是否连接已经连接成功的这个connection，第二个是查看是否连接成功
		       const boost::system::error_code &error){
        if(!error){    //如果没有失败就可以开始干活了
	    new_connection->start();
	}

        start_accept();   //不管连接成功或失败都要去连接一个新客户端
    }

    tcp::acceptor acceptor_;
}

void main(){    //通常服务器都是这么简单的，生成一个io_service构造一个server，然后调用io_service.run()完事
    try{
	boost::asio:io_service io_service;
        tcp_server server(io_service);
	io_service.run();
    }
    catch(std::exception &e){ 
	std::cerr << e.what() << std::endl;
    }
    return 0;
}













