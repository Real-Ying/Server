#ifndef __WANGDAO_RSSREADER_H__
#define __WANGDAO_RSSREADER_H__

#include "tinyxml2.h"
#include <string>
#include <vector>
#include <memory>

using std::string;
using std::vector;
using std::shared_ptr;
using namespace tinyxml2;

//保存每个item的结构体数据
struct RssItem
{
	string _title;
	string _link;
	string _pubDate;
	string _content;
};

struct RssData
{
	string _version;
	string _title;
	string _link;
	string _description;
	string _language;
	vector<shared_ptr<RssItem> > _items;
};

class RssReader
{
public:
	RssReader(vector<string> & files);
	~RssReader();

	void loadFeedFiles();
	void loadFeedFile(const string & filename);

	void initWithRssString(const string & rss);
	void makePages(vector<string> & pages);
private:
	void parseRss(XMLDocument & doc);   //rss解析函数只供类内部函数使用，设为私有
	//string debug();
	//void dump(const string & filename);
private:
	vector<shared_ptr<RssItem> > _items;   //存放解析后的item的vector
	vector<string> & _files;   //存放传入数个rss文件的vector
};

#endif
