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
		 
};

void Dictionary::read(const string& filename) {
	ifstream ifs(filename);
    if(!ifs) {
		cout << file open error <<endl;
		return;
	}	
	string line;
	while(getline(ifs,line)) {  //当获得的行内容不为空时
		istringstream iss(line);  //把这行内容拿给字符串IO处理
		string preword;  //未经处理的单词
		while(iss >> preword) {
		    string word = preprocessWord(preword);  //调单词预处理函数对未处理单词进行处理	
		}
	}
}













