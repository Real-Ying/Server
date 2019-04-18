
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
	void store();     //存储网页库和位置偏移库

private:
	Configuration &          _conf;
	DirScanner &             _dirScanner;
	std::vector<std::string> _vecPages;
};

}//end of namespace wd

#endif
