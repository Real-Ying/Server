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
  bool operator<(const Record& rhs) const {  //下面对Vector中的Record排序涉及比较问题（error信息中具体要求<重载)
    return freq > rhs.freq;                  //这里<重载为根据频率大小比较(也可以首字母大小或其他)
  }	
};

class Dictionary {
 public:
  Dictionary() {
    _dict.reserve(10000);  //设置_dict的预留容量
 }
  
  void read(const string& filename);
  void store(const string& filename);

 private:
  string preprocessWord(string& word); 
  void statistic(const string& word); 
 private: 
  vector<Record> _dict;
};

void Dictionary::read(const string& filename) {
  ifstream ifs(filename);
  if (!ifs) {
    cout << "file open error" <<endl;
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
  std::sort(_dict.begin(), _dict.end());  //完成后根据频率大小对索引vextor进行快排
}

void Dictionary::store(const string& word) {
  ofstream ofs(filename);  //创建一个文件输出流
  if (!ofs) {
      cout << " >> ofstream open error!" << endl;
      return;
  }
  for(auto& record : _dict) {  //将_dict里的内容以此种格式输入到文件中
      ofs << record.word << "\t" << record.freq << "\n";
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


void Dictionary::statistic(const string& word) {
  size_t idx;
  for(idx = 0; idx != _dict.size(); ++idx) {
    if (word == _dict[idx].word) {  //如果在_dict中能找到这个word(统计过)，则_dict中该词频率加1
        ++_dict[idx].freq;
        break;
    }
  }
				
    if (idx == _dict.size()){      //没有找到(从未统计过),则建立一个新的Record对象并推进_dict
        Record record;
	record.word = word;
	record.freq = 1;
	_dict.push_back(record);
    }
}

int main() {
  Dictionary dictionary;
  dictionary.read("The_Holy_Bible.txt");
  dictionary.store("dict.txt");
  return 0;
}







