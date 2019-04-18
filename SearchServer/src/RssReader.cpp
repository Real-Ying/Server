
#include "RssReader.hpp"
 
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/regex.hpp>
using std::cout;
using std::endl;
using std::ostringstream;
using std::ofstream;

//构造函数 构造时传入文件流(多个文件) 并打印日志
RssReader::RssReader(vector<string> & files)
: _files(files)
{
	cout << "RssReader()" << endl;
}

//析构函数 析构时打印日志
RssReader::~RssReader()
{
	cout << "~RssReader()" << endl;
}

//加载并解析传入进来的数个rss文件
void RssReader::loadFeedFiles()
{
	for(size_t idx = 0; idx != _files.size(); ++idx)
	{
		loadFeedFile(_files[idx]);
		cout << "has load " << idx + 1 << " rss files" << endl;    //每次完成一个文件解析已完成数日志增加
	}
}

//加载一个rss文件
void RssReader::loadFeedFile(const string & filename)
{
	XMLDocument doc;    //创建一个xml文件变量再加载进文件
	doc.LoadFile(filename.c_str());   //c_str()将C++的string字符串转化为C的字符串
	cout << "loadFile: " << filename << endl;    //打印当前载入的文件名
	XMLError errorID = doc.ErrorID();    //返回0表示载入成功
	if(errorID)
	{
		cout << "XMLDocument LoadFile error!" << endl;
		return ;
	}
	else
	{
		parseRss(doc);    //解析该rss文件
	}
}

//将解析vector进行格式化拼接
void RssReader::makePages(vector<string> & pages)
{
	cout << "pages's size = " << _items.size() << endl;
	for(size_t idx = 0; idx != _items.size(); ++idx)
	{
		ostringstream oss;
		oss << "<doc>\n"
			<< "  <docid>" << (idx + 1) << "</docid>\n"
			<< "  <title>" << _items[idx]->_title << "</title>\n"
			<< "  <link>"  << _items[idx]->_link << "</link>\n" 
			<< "  <content>" << _items[idx]->_content << "</content>\n"
			<< "/<doc>\n";
		
		string page = oss.str();
		pages.push_back(page);    //将格式化拼接后的item push进vector(page)
	}
}

//解析一个rss文件
void RssReader::parseRss(XMLDocument & doc)
{
	cout << "parsRss(XMLDocument & doc)" << endl;

	//通过递归方式找到第一篇文章 用指针指向
	XMLElement * root = doc.FirstChildElement();
	XMLElement * channel = root->FirstChildElement("channel");
	XMLElement * item = channel->FirstChildElement("item");
	
	//从第一个item开始，遍历完这篇文章所有item结点
	for(; item; item = item->NextSiblingElement())    //相当于while(item){...item->NextSiblingElement()}
	{
		//用智能指针申请一个RssItem结构体数据的空间，并将item结点的子结点数据逐一分配 
		shared_ptr<RssItem> rssItemPtr(new RssItem);    
		const char * itemTitle = item->FirstChildElement("title")->GetText();  //GetText()函数用获取某元素的文本信息复制到一个内存变量
		const char * itemLink = item->FirstChildElement("link")->GetText();
		const char * itemDescription = item->FirstChildElement("description")->GetText();
                
		//有content:encoded则用content:encoded，没有则用Description
		const char * iContent = nullptr;    //第三方指针选择两者其中一个的文本指向
		XMLElement * contentEncoded = item->FirstChildElement("content:encoded");   //这里要判定结点是否存在再取其内容
		if(contentEncoded)    
		{	iContent = contentEncoded->GetText();	}
		else
		{	iContent = itemDescription; }
                
	        //将content:encoded 或 Description的文本进行html标注的过滤	
		//string szReg = "<(\S*?)[^>]*>";//可行
		string szReg = "<.*?>";//可行 去除所有html的正则表达式Google
		boost::regex fmt(szReg);    //定义一个正则表达式变量
		string content = boost::regex_replace(string(iContent), fmt, string(""));    //用空格替换掉匹配到的html标注
		
		//打印该item的子结点
		rssItemPtr->_title = itemTitle ? itemTitle : string("");
		rssItemPtr->_link = itemLink ? itemLink : string("");
		rssItemPtr->_content = content;
                
		//将完成的Rssitem push进vector保存
		_items.push_back(rssItemPtr);
	}
}

#if 0
string RssReader::debug()
{
	ostringstream oss;
#if 1
	oss << "--- <RssData> ---\n"
		<< "Version: " << _rssDataPtr->_version << "\n"
		<< "Language: " << _rssDataPtr->_language << "\n"
		<< "Link: " << _rssDataPtr->_link << "\n"
		<< "Title: " << _rssDataPtr->_title << "\n"
		<< "Description: " << _rssDataPtr->_description << "\n"
		<< "LastBuildDate: " << _rssDataPtr->_lastBuildDate << "\n"
		<< "  <items>  " << "\n";
#endif
	
	for(size_t idx = 0; idx != _rssDataPtr->_items.size(); ++idx)
	{
		oss << "  ---items[" << idx << "]---\n" 
			<< "  items[" << idx << "].title: " << _rssDataPtr->_items[idx]->_title << "\n"
			<< "  items[" << idx << "].link: " << _rssDataPtr->_items[idx]->_link << "\n"
		//	<< "  items[" << idx << "].guid: " << _rssDataPtr->_items[idx]->_guid << "\n"
		//	<< "  items[" << idx << "].pubDate: " << _rssDataPtr->_items[idx]->_pubDate << "\n"
		//	<< "  items[" << idx << "].description: " << _rssDataPtr->_items[idx]->_description << "\n"
			<< "  items[" << idx << "].content: " << "\n" 
			<< _rssDataPtr->_items[idx]->_content << "\n";
	}

	oss	<< "--- </RssData> ---\n";

	return oss.str();
}	

void RssReader::dump(const string & filename)
{
	ofstream ofs(filename.c_str());
	if(!ofs.good())
	{	cout << "ofstream open error!" << endl;	}

	ofs << debug();
	ofs.close();
}
#endif 
