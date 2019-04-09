#include"tinyxml2.h"
#include<iostream>
using std::cout;
using std::endl;
using namesapce tinyxml2;

void test0(void)
{
    XMLDocument doc;    //先创建一个文件变量再加载进文件
    doc.LoadFile("");
    if(doc.ErrorID())   //返回0表示加载成功
    { 
        cout<<"xmlfile load fail"<<endl;
    }
    
}


