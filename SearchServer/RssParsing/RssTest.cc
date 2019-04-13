#include"tinyxml2.h"
#include<iostream>
#include<string>
#include<boost/regex.hpp>

using std::cout;
using std::endl;
using std::string;
using namespace tinyxml2;

/*
struct RssItem
{
    string titl;
    string link;
    string description;
    string content;
};
*/
void test0(void)
{
    XMLDocument doc;    //先创建一个文件变量再加载进文件
    doc.LoadFile("coolshell.xml");
    if(doc.ErrorID())   //返回0表示加载成功
    { 
        cout<<"xmlfile load fail"<<endl;
    }
    XMLElement * itemNode = doc.FirstChildElement("rss")    //通过递归遍历找到第一篇文章 用指针itemNode指向
	   	        	->FirstChildElement("channel")
				->FirstChildElement("item");	
    // if(itemNode)    //提取一个item的内容时
    while(itemNode)    //提取一个文件的所有item
    {   //如果指向的这篇文章存在 则去拿下面的兄弟节点 
        XMLElement * titleNode = itemNode->FirstChildElement("title");
	XMLElement * linkNode = itemNode->FirstChildElement("link");
	XMLElement * descriptionNode = itemNode->FirstChildElement("description");
        XMLElement * contentNode = itemNode->FirstChildElement("content:encoded");
        
	//拿到兄弟节点指针后就去获取里面的字符串
        string title(titleNode->GetText());
	string link(linkNode->GetText());
	string description_(descriptionNode->GetText());
	string content_(contentNode->GetText());
        
	cout<<"title --> "<<title<<endl;
	cout<<"link  --> "<<link<<endl;
	//cout << "description: --> " << description << endl;
	//cout << "content: --> " << content << endl;
       


        /***使用正则表达式过滤掉content里的html标记
	******************************************/
	//传入正则表达式字符串 创建一个正则表达式对象
        //正则表达式需要拼凑使之能够匹配所有的html的标签，这里直接google一个
	boost::regex re("</?[^>]+>");  
        
	//结果发现 description里也有Html标注 去掉之
        string description = boost::regex_replace(description_, re, string(""));
        cout << "description --> " << description << endl;

	//例子中boost::regex_replace()的这种传参用法的返回值是字符串正是我所需的
	//参数1.原字符串 2.正则表达式对象 3.替换内容(这里是替换成空字符串)
	string content = boost::regex_replace(content_,re,string(""));
	cout << "content --> " << content << endl;


        itemNode = itemNode -> NextSiblingElement("item");    //itemNode->NestSiblingElement(“item”) 得到itemNode节点标签为item的兄弟节点

    } 
    
}

int main()
{
    test0();
    return 0;
}
//g++ RssTest.cc tinyxml.cc 编译
// ./a.out  运行
// 使用正则表达式后编译时需要加上boost::regex库的链接，即添加 -lboost_regex
