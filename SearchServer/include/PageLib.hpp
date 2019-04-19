
#ifndef _WD_PAGELIB_H_
#define _WD_PAGELIB_H_

#include <string>
#include <vector>


namespace wd
{

class Configuration;
class DirScanner;

class PageLib
{
public:
	PageLib(Configuration & conf,
			DirScanner & dirScanner);

	void create();    //创建网页库(即调用RssReader对象函数)
	void store();     //存储网页库文件和位置偏移库文件

private:
	Configuration &          _conf;
	DirScanner &             _dirScanner;  
	std::vector<std::string> _vecPages;    //内存中的网页库(由makePage()生成)
};

}//end of namespace wd

#endif
