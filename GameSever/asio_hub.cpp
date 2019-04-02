
/*********************同步风格**********************/
#include<iostream>
#include<boost/asio.hpp>
#include<boost/date_time/posix_time/posix_time.hpp>
int main()
{
    boost::asio::io_service io;   //要使用asio功能必须首先定义一个io_service对象
    boost::asio::deadline_timer t(io,boost::posix_time::seconds(5));  //用老的时间库定义一个5s定时器
    t.wait(); //启动定时器        //参数接口是一个是io_service对象因为他是OS底层函数和代码的中间件 另一个是时间函数
                                  //同步这里造成了阻塞
    std::cout << "hello world!" << std::endl;  
    return 0;    
}

/********************异步风格***********************/
void callback(const boost::system::error_code&)
{
    std::cout << "hello world!" <<std::endl;	
}

int main()
{
    boost::asio::io_service io;
    boost::asio::deadline_timer t(io,boost::posix_time::sencods(5));
    t.async_wait(callback);  //这是异步的启动有一个回调函数参数，启动定时器时立即返回去做其他事不阻塞，
                             //只是在5秒之后的一个时机去调用参数里的回调函数，2行代码5秒之后调用回调相当于一个事件
    std::cout << "start run\n" <<std::endl;    
    io.run;  //这个时机就是run循环到了这个事件  这个回调是在run里执行的，run循环停止有各种情况1.IO里调用stop()2.对应消息池里面没有事件了都处理完了
    std::cout << "finish run\n" <<std::endl;
    return 0;  //这里的io只有一个事件处理完毕就退出了
}
//result is "start run" first, "helle world!" then，"finish run" last.

/***************异步下的两事件更明显的非阻塞*****************/

void callback1(const boost::system::error_code&)
{
    std::cout << "hello world!1" <<std::endl;	
}

void callback2(const boost::system::error_code&)
{
    std::cout << "hello world!2" <<std::endl;	
}

int main()
{
    boost::asio::io_service io;
   
    boost::asio::deadline_timer t1(io,boost::posix_time::sencods(5));
    t1.async_wait(callback);                              
    boost::asio::deadline_timer t1(io,boost::posix_time::sencods(2));
    t2.async_wait(callback);                              
   
    std::cout << "start run\n" <<std::endl;    
    io.run;  
    std::cout << "finish run\n" <<std::endl;
    return 0;  
}
//运行结果是 “start run” “hello world!1” “hello world!2” “finish run”.







