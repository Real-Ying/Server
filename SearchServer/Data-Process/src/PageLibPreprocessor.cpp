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
    //开辟字符串流缓存保存网页偏移库每行，将每行各项逐一放入内存字符变量
    stringstream ss(line);
    ss >> docId >> docOffset >> docLen;
    
    //根据偏移量从每篇正向索引里提取出内容，用来构造webpage对象形成新的网页库    
    string doc;
    doc.resize(docLen, ' ');  //string::resize()调整字符串大小，预留该文章大小空间
    pageIfs.seekg(docOffset, pageIfs.beg);  //seek(off, dir) off相对偏移量，dir偏移的起始位置，这里是
    pageIfs.read(&*doc.begin(), docLen);    //read(char *s, len) *s是doc字符数组首地址，&每次read后改变位置
   
    WebPage webPage(doc, _conf, _jieba); //传入此篇文章内容(及配置和分词类对象)构造一个WebPage对象,得到这篇文章top词集
    _pageLib.push_back(webPage);      //推入网页库容器(包含webpage里的词频统计map和top词集的，非PageLib类处理的网络库)

    _offsetLib.insert(std::make_pair(docId, std::make_pair(docOffset, docLen)));  //以此种容器形式保存此篇文章正向索引
  }

#if 0
  for (auto mit : _offsetLib) {
    cout << mit.first << "\t" << mit.second.first << "\t" << mit.second.second << endl;
  }
#endif
}


void PageLibPreprocessor::cutRedundantPages() {
  for (size_t i = 0; i != _pageLib.size() - 1; ++i) {  //从_PageLib向量第一个开始查找重复文章
    for(size_t j = i + 1; j != _pageLib.size(); ++j) {
      if (_pageLib[i] == _pageLib[j]) {  //这里的 == 是在WebPage中判断两个top向量是否相等的==重载
        //删除重复元素可以逐一向前移位但时间成本很大，由于这里不必保持元素顺序 所以采用用末尾元素替换被删除元素然后pop_back()删除末尾元素的方式
	_pageLib[j] = _pageLib[_pageLib.size() - 1];  
	_pageLib.pop_back();
	--j;  //末位元素被删所以循环条件变化，.size()-1了但改不了size所以改j来改变条件
      }
    }
  }
}

void PageLibPreprocessor::buildInvertIndexTable() {
  //遍历重构后的网络库,以获取词频map的vector，以此重构词频map
  //map<string, int(freq)> wordFreq ——> unordered_map<string, vector<std::pair<int(docid), double(freq)>>> wordFreq
  for (auto page : _pageLib) {  
    map<string, int>& wordsMap = page.getWordsMap();  
    for (auto wordFreq : wordsMap) {
      _invertIndexTable[wordFreq.first].push_back(std::make_pair(page.getDocId(), wordFreq.second));  
    }
  }
	
  //计算每篇文档中的词的权重,并归一化
  map<int, double> weightSum;// 归一化步骤分母所需 map<docid, freq> 保存一篇文档中所有词的权重平方和. w1^2+w2^2+……+wn^2
  //tf-idf算法 迭代器都用& 最终结果就是改变后的_PageLib
  int totalPageNum = _pageLib.size();       //N
  for (auto & item : _invertIndexTable) {   	
    int df = item.second.size();            //df vector的长度就是这个词在所有文章中出现的次数,item.second即vector
    //求关键词item.first的逆文档频率
    double idf = log(static_cast<double>(totalPageNum)/ df + 0.05) / log(2);  //idf
		
    for (auto & sitem : item.second) {  //item.second=vector
      double weight = sitem.second * idf;   //w=tf*idf tf=freq=sitem.second

      weightSum[sitem.first] += pow(weight, 2); //同时填充weightSum容器
      sitem.second = weight;                    //w替换掉重构的词频map元素freq以得到 
    }
  }

  for (auto & item : _invertIndexTable) {	
  //归一化处理
    for (auto & sitem : item.second) {  //item.second.second 即weight
      sitem.second = sitem.second / sqrt(weightSum[sitem.first]);  //将w项进行归一化计算得到weight，从而最终得到 倒排索引表
    }
  }


#if 0 // for debug 打印_invertIndexTable的内容
  for (auto item : _invertIndexTable) {
    cout << item.first << "\t";
    for (auto sitem : item.second) {
      cout << sitem.first << "\t" << sitem.second <<  "\t";
    }
    cout << endl;
  }
#endif
}


void PageLibPreprocessor::storeOnDisk() {
  sort(_pageLib.begin(), _pageLib.end());  //新网络库排序一下
  
  //存新网页库和新网页位置偏移库
  ofstream ofsPageLib(_conf.getConfigMap()[NEWPAGELIB_KEY].c_str());  //以配置中新网页库和新位置偏移库的Key对应值(路径)为参数创建两个文件输出流
  ofstream ofsOffsetLib(_conf.getConfigMap()[NEWOFFSETLIB_KEY].c_str());
  if (!ofsPageLib.good() || !ofsOffsetLib.good()) {	
    cout << "new page or offset lib ofstream open error!" << endl;
  }
  
  for (auto & page : _pageLib) {
    int id = page.getDocId();
    int length = page.getDoc().size();
    ofstream::pos_type offset = ofsPageLib.tellp();
    ofsPageLib << page.getDoc();

    ofsOffsetLib << id << '\t' << offset << '\t' << length << '\n';
  }

  ofsPageLib.close();
  ofsOffsetLib.close();

  //存倒排索引表
  ofstream ofsInvertIndexTable(_conf.getConfigMap()[INVERTINDEX_KEY].c_str());
  if (!ofsInvertIndexTable.good()) {
    cout << "invert index table ofstream open error!" << endl;
  }
  for (auto item : _invertIndexTable) {
    ofsInvertIndexTable << item.first << "\t";
    for (auto sitem : item.second) {
      ofsInvertIndexTable << sitem.first << "\t" << sitem.second <<  "\t";
    }
    ofsInvertIndexTable << endl;  // <<endl 换行并立即刷新缓冲区 相当于/n + <<flush
  }
  ofsInvertIndexTable.close();
}

}// end of namespace wd
