#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
#include <algorithm>
using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;
using std::ofstream;
using std::istringstream;

struct Record {
	string word;
	int freq;
};

class Dictionary {
 public:
  void read(const string& filename);
  void store(const string& filename);

 private:
  vector<Record> _dict;
  string preprocessWord(const string& );  
		 
};

void Dictionary::read(const string& filename) {
  ifstream ifs(filename);
  if (!ifs) {
    cout << file open error <<endl;
    return;
  }	
  string line;
  while (getline(ifs,line)) {  //逐行读取文件内容存入内存空间变量line
    istringstream iss(line);  //以line创建字符串输入流
    string preword;  //未经处理单词
    while (iss >> preword) {  //字符串输入流中读取未经处理的单词，>> 不可读入不可见字符起到过滤空格作用，对每个word进行 预处理 和 统计
      string word = preprocessWord(preword);  //调单词预处理函数对未处理单词进行处理	
      if (word != string())  //处理后的单词不是因为包含数字返回的空字符串则进行统计
          statistic(word);
  }
}

string Dictionary::preprocessWord(string& word) {  //对单词进行预处理
  for(size_t idx = 0; idx != word.size(); ++idx) { //size_t是无符号的int
    if (isdigit(word[idx])) {  //单词中有数字字母存在则该位返回空字符串替代
        return string();
    } 
    else if (isupper(word[idx]))  //字母大写转小写  
    {
        word[idx] = tolower(word[idx]);
    }
  }
  return word;  //返回处理后的单词
}

void Dictionary::statistic(const string& word)
{
  for(size_t idx = 0; idx != _dict.size(); ++idx) {
    if (word == _dict[idx].word) { //如果能够查找到
        ++_dict[idx].freq;
        break;
    }
  }
				
    if (idx == _dict.size()) {//没有找到
        Record record;
	record.word = word;
	record.freq = 1;
	_dict.push_back(record);
    }
}









