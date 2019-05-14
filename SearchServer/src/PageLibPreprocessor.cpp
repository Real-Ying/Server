#include "PageLibPreprocessor.hpp"
#include "GlobalDefine.hpp"

#include <stdio.h>
#include <time.h>
#include <fstream>
#include <sstream>

using std::ifstream;
using std::ofstream;
using std::stringstream;

namespace wd {
//构造函数
PageLibPreprocessor::PageLibPreprocessor(Configuration & conf)
    : _conf(conf) {

}

void PageLibPreprocessor::doProcess() {
  readInfoFromFile();       // 根据配置信息读取网页库和位置偏移库的内容
  time_t t1 = time(NULL);
  cutRedundantPages();      //去重冗余的网页数据
  buildInvertIndexTable();  //创建倒排索引库(容器)  
  time_t t2 = time(NULL);
  printf("preprocess time: %d min\n", (t2 - t1)/60);  //打印以上执行耗时单位min

  storeOnDisk();            //三个库存磁盘
  time_t t3 = time(NULL);
  printf("store time: %d min\n", (t3 - t2)/60);       //打印存盘耗时
}

void PageLibPreprocessor::readInfoFromFile() {
  //获取配置文件中的 格式化网页库 和 网页偏移库 路径
  map<string, string> &configMap = _conf.getConfigMap();
  string pageLibPath = configMap[RIPEPAGELIB_KEY];
  string offsetLibPath = configMap[OFFSETLIB_KEY];
  
  //为两个库创建文件输入流(缓存区)
  ifstream pageIfs(pageLibPath.c_str());
  ifstream offsetIfs(offsetLibPath.c_str());
  if ((!pageIfs.good()) || (!offsetIfs.good())) {
    cout << "page or offset lib open error" << endl;
  }

  //逐行读网页偏移库各个项目 
  string line;
  int docId, docOffset, docLen;
  while (getline(offsetIfs, line)) {
    //开辟字符串流缓存，偏移库内容行逐一放入内存字符变量
    stringstream ss(line);
    ss >> docId >> docOffset >> docLen;
    
    //根据偏移量    
    string doc;
    doc.resize(docLen, ' ');  //string::resize()调整字符串大小，预留该文章大小
    pageIfs.seekg(docOffset, pageIfs.beg);  //
    pageIfs.read(&*doc.begin(), docLen);
   
    WebPage webPage(doc, _conf, _jieba);  //对  
    _pageLib.push_back(webPage);

    _offsetLib.insert(std::make_pair(docId, std::make_pair(docOffset, docLen)));
  }
#if 0
  for (auto mit : _offsetLib) {
    cout << mit.first << "\t" << mit.second.first << "\t" << mit.second.second << endl;
  }
#endif
}

void PageLibPreprocessor::cutRedundantPages()
{
	for(size_t i = 0; i != _pageLib.size() - 1; ++i)
	{
		for(size_t j = i + 1; j != _pageLib.size(); ++j)
		{
			if(_pageLib[i] == _pageLib[j])
			{
				_pageLib[j] = _pageLib[_pageLib.size() - 1];
				_pageLib.pop_back();
				--j;
			}
		}
	}
}

void PageLibPreprocessor::buildInvertIndexTable()
{
	for(auto page : _pageLib)
	{
		map<string, int> & wordsMap = page.getWordsMap();
		for(auto wordFreq : wordsMap)
		{
			_invertIndexTable[wordFreq.first].push_back(std::make_pair(
					page.getDocId(), wordFreq.second));
		}
	}
	
	//计算每篇文档中的词的权重,并归一化
	map<int, double> weightSum;// 保存每一篇文档中所有词的权重平方和. int 代表docid

	int totalPageNum = _pageLib.size();
	for(auto & item : _invertIndexTable)
	{	
		int df = item.second.size();
		//求关键词item.first的逆文档频率
		double idf = log(static_cast<double>(totalPageNum)/ df + 0.05) / log(2);
		
		for(auto & sitem : item.second)
		{
			double weight = sitem.second * idf;

			weightSum[sitem.first] += pow(weight, 2);
			sitem.second = weight;
		}
	}

	for(auto & item : _invertIndexTable)
	{	//归一化处理
		for(auto & sitem : item.second)
		{
			sitem.second = sitem.second / sqrt(weightSum[sitem.first]);
		}
	}


#if 0 // for debug
	for(auto item : _invertIndexTable)
	{
		cout << item.first << "\t";
		for(auto sitem : item.second)
		{
			cout << sitem.first << "\t" << sitem.second <<  "\t";
		}
		cout << endl;
	}
#endif
}

void PageLibPreprocessor::storeOnDisk()
{
	sort(_pageLib.begin(), _pageLib.end());	

	ofstream ofsPageLib(_conf.getConfigMap()[NEWPAGELIB_KEY].c_str());
	ofstream ofsOffsetLib(_conf.getConfigMap()[NEWOFFSETLIB_KEY].c_str());

	if( !ofsPageLib.good() || !ofsOffsetLib.good())
	{	
		cout << "new page or offset lib ofstream open error!" << endl;
	}

	for(auto & page : _pageLib)
	{
		int id = page.getDocId();
		int length = page.getDoc().size();
		ofstream::pos_type offset = ofsPageLib.tellp();
		ofsPageLib << page.getDoc();

		ofsOffsetLib << id << '\t' << offset << '\t' << length << '\n';
	}

	ofsPageLib.close();
	ofsOffsetLib.close();


	// invertIndexTable
	ofstream ofsInvertIndexTable(_conf.getConfigMap()[INVERTINDEX_KEY].c_str());
	if(!ofsInvertIndexTable.good())
	{
		cout << "invert index table ofstream open error!" << endl;
	}
	for(auto item : _invertIndexTable)
	{
		ofsInvertIndexTable << item.first << "\t";
		for(auto sitem : item.second)
		{
			ofsInvertIndexTable << sitem.first << "\t" << sitem.second <<  "\t";
		}
		ofsInvertIndexTable << endl;
	}
	ofsInvertIndexTable.close();
}

}// end of namespace wd
