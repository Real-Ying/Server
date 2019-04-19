
#include "Configuration.hpp"
#include "PageLib.hpp"
#include "DirScanner.hpp"
#include "RssReader.hpp"
#include "GlobalDefine.hpp"

#include <iostream>
#include <fstream>

namespace wd
{
//构造函数
PageLib::PageLib(Configuration & conf,
	         DirScanner & dirScanner)
	: _conf(conf),
	  _dirScanner(dirScanner)
{}

//由目录扫描类传入数个Rss文件，对其进行RssReader的操作，生成网页库
void PageLib::create()
{
	std::vector<std::string> & vecFiles = _dirScanner.files();    //由dirScanner类得到数个Rss文件
	RssReader reader(vecFiles);
	reader.loadFeedFiles();
	reader.makePages(_vecPages);
}

void PageLib::store()
{
	//读取配置信息(即两个输出文件流对象：网页库文件ofsPage 和 正向索引库ofsOffset)
	std::map<std::string, std::string> & confMap = _conf.getConfigMap();    //获取配置文件路径(再Configuration类中以map<路径，路径>形式存储)
	std::string pageLibPath = confMap[RIPEPAGELIB_KEY];    //分别提取map中文件的两个路径
	std::string offsetLibPath = confMap[OFFSETLIB_KEY];

	std::ofstream ofsPage(pageLibPath.c_str());    //以两个路径为参创建两个ofstream对象
	std::ofstream ofsOffset(offsetLibPath.c_str());
	if(!(ofsPage.good() && ofsOffset.good()))      //查看两个文件输出流对象状态，非同时good时日志报错
	{
		std::cout << "ofstream open error!" << std::endl;
		return;
	}
        
	//将creat()生成的格式化拼接vector 和 得到的其中每个Rssitem(每篇文章)的正向索引(docid、offset、length) 分别输出到两个文件
	for(size_t idx = 0; idx != _vecPages.size(); ++idx)
	{
		int id = idx + 1;    //vector下标0开始
		int length = _vecPages[idx].size();    //.size()获取格式化拼接后的每个item长度
		std::ofstream::pos_type offset = ofsPage.tellp();    //.tellp()获取文件流(当前vector元素)写入时的起始指针
		
		//分别输出到两个文件
		ofsPage << _vecPages[idx];
		ofsOffset << id << '\t' << offset << '\t' << length << '\n';
	}

	ofsPage.close();
	ofsOffset.close();
}

}//end of namespace wd
