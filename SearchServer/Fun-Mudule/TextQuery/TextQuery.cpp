/*
 * This file contains code from "C++ Primer, Fifth Edition", by Stanley B.
 * Lippman, Josee Lajoie, and Barbara E. Moo, and is covered under the
 * copyright and warranty notices given in that book:
 * 
 * "Copyright (c) 2013 by Objectwrite, Inc., Josee Lajoie, and Barbara E. Moo."
 * 
 * 
 * "The authors and publisher have taken care in the preparation of this book,
 * but make no expressed or implied warranty of any kind and assume no
 * responsibility for errors or omissions. No liability is assumed for
 * incidental or consequential damages in connection with or arising out of the
 * use of the information or programs contained herein."
 * 
 * Permission is granted for this code to be used for educational purposes in
 * association with the book, given proper citation if and when posted or
 * reproduced.Any commercial use of this code requires the explicit written
 * permission of the publisher, Addison-Wesley Professional, a division of
 * Pearson Education, Inc. Send your request for permission, stating clearly
 * what code you would like to use, and in what specific way, to the following
 * address: 
 * 
 * 	Pearson Education, Inc.
 * 	Rights and Permissions Department
 * 	One Lake Street
 * 	Upper Saddle River, NJ  07458
 * 	Fax: (201) 236-3290
*/ 

#include "TextQuery.h"
#include "make_plural.h"

#include <cstddef>
#include <memory>  // 
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <cctype>
#include <cstring>
#include <utility>

using std::size_t;
using std::shared_ptr;
using std::istringstream;
using std::string;
using std::getline;
using std::vector;
using std::map;
using std::set;
using std::cerr;
using std::cout;
using std::cin;
using std::ostream;
using std::endl;
using std::ifstream;
using std::ispunct;
using std::tolower;
using std::strlen;
using std::pair;

// because we can't use auto, we'll define typedefs 
// to simplify our code

// type of the lookup map in a TextQuery object
typedef map<string, shared_ptr<set<TextQuery::line_no> > > wmType;

// the iterator type for the map
typedef wmType::const_iterator wmIter;

// type for the set that holds the line numbers
typedef shared_ptr<set<TextQuery::line_no> > lineType;

// iterator into the set
typedef set<TextQuery::line_no>::const_iterator lineIter;

//文章预处理功能 读取文章并装载 (元素类型为 单词 --> 行数1...行数n 的set 的vector容器  并由share_ptr指向
TextQuery::TextQuery(ifstream &is)  //传入的参数是一个文件输入流，这样传参实则没必要，完全可以在函数内部创建 因为也只在此函数内部用
    : file(new vector<string>) {    //file是一个share_ptr指向所以new一个空字符串向量让它指向
    string text;
    while (getline(is, text)) {     // 对文件输入流其中的内容逐行读取
      file->push_back(text);        // 在内存中用vec保存这一行
      int n = file->size() - 1;     // n是当前行的行号 即当前file(vec)有多少个元素就有几行这里剪了个1
      istringstream line(text);     // 将此行放入字符串输入流缓存等待处理
      string word;               
        while (line >> word) {      //对行中每个词进行处理 由于>>不读不可显字符起到分割作用
          word = cleanup_str(word);    //如果词不是标点符号则转化为小写
          //map<string(word), share_ptr<set(lines)>> 以word作为键值查找该词是否出现过
          lineType &lines = wm[word]; // lineType宏定义过是指向set的share_ptr
          if (!lines) // 如果这个指针指向null即word未曾出现 则为其重新new一个set用reset方法分配给智能指针指向的lines 再把行号值n插进set去
            lines.reset(new set<line_no>);  //出现过则直接放n进set去
          lines->insert(n);      
	}
    }
}

//word转化为小写
string TextQuery::cleanup_str(const string &word) {
  string ret;
  for (string::const_iterator it = word.begin(); it != word.end(); ++it) {
    if (!ispunct(*it))    //迭代器指向的字符不为标点符号时
      ret += tolower(*it);
  }
    return ret;
}

//查询功能
QueryResult              //返回的是QueryResult对象
TextQuery::query(const string &sought) const {
  static lineType nodata(new set<line_no>);   // 可能查询失败 所以备一个share_ptr指向的空set以返回
  
  //查询词转化为小写后进行查找，找到则返回QueryResult()得到的结果,找不到则将nodata传入返回
  wmIter loc = wm.find(cleanup_str(sought));  
  if (loc == wm.end()) 
    return QueryResult(sought, nodata, file);  
  else 
    return QueryResult(sought, loc->second, file);
}

ostream &print(ostream & os, const QueryResult &qr) {
  // if the word was found, print the count and all occurrences
  os << qr.sought << " occurs " << qr.lines->size() << " "
                  << make_plural(qr.lines->size(), "time", "s") << endl;

  // print each line in which the word appeared
  // for every element in the set 
  for (lineIter num = qr.lines->begin();
    num != qr.lines->end(); ++num) 
  // don't confound the user with text lines starting at 0
  os << "\t(line " << *num + 1 << ") " 
		   << *(qr.file->begin() + *num) << endl;
  return os;
}

// debugging routine, not covered in the book
void TextQuery::display_map() {
  wmIter iter = wm.begin(), iter_end = wm.end();

  // for each word in the map
  for ( ; iter != iter_end; ++iter) {
    cout << "word: " << iter->first << " {";

  // fetch location vector as a const reference to avoid copying it
  lineType text_locs = iter->second;
  lineIter loc_iter = text_locs->begin(),
           loc_iter_end = text_locs->end();

    // print all line numbers for this word
    while (loc_iter != loc_iter_end) {
      cout << *loc_iter;

      if (++loc_iter != loc_iter_end)
        cout << ", ";
    }
  cout << "}\n";  // end list of output this word
  }
  cout << endl;  // finished printing entire map
}

