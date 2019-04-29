#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
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
    while (iss >> preword) {  //字符串输入流中读取未经处理的单词，>> 不可读入不可见字符起到过滤空格作用
      string word = preprocessWord(preword);  //调单词预处理函数对未处理单词进行处理	
    }
  }
}

string Dictionary::preprocessWord(string& word) {

}











