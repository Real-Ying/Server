#include"tinyxml2.h"
#include<iostream>
#include<string>
using std::cout;
using std::endl;
using std::string;
using namespace tinyxml2;

void test0(void)
{
    XMLDocument doc;    //先创建一个文件变量再加载进文件
    doc.LoadFile("");
    if(doc.ErrorID())   //返回0表示加载成功
    { 
        cout<<"xmlfile load fail"<<endl;
    }
    XMLElement * itemNode = doc.FirstChildElement("rss")    //通过递归遍历找到第一篇文章 用指针itemNode指向
	   	        	->FirstChildElement("channel")
		       		->FirstChildElement("item");	
    if(itemNode)    
    {   //如果指向的这篇文章存在 则去拿下面的兄弟节点 
        XMLElement * titleNode = itemNode->FirstChildElement("title");
	XMLElement * linkNode = itemNode->FirstChildElement("link");
	XMLElement * descriptionNode = itemNode->FirstChildElement("description");
        XMLElement * contentNode = itemNode->FirstChildElement("content:encoded");
        
	//拿到兄弟节点指针后就去获取里面的字符串
        string title(titleNode->GetText());
	string link(linkNode->GetText());
	string description(descriptionNode->GetText());
	string content(contentNode->GetText());

	cout<<"title --> "<<title<<endl;
	cout<<"link  -->"<<link<<endl;
    } 
}

int main()
{
    test0();
    return 0;
}

