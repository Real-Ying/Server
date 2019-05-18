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

#ifndef QUERYRESULT_H
#define QUERYRESULT_H
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <iostream>

class QueryResult {
friend std::ostream& print(std::ostream&, const QueryResult&);
 public:
  typedef std::vector<std::string>::size_type line_no;
  typedef std::set<line_no>::const_iterator line_it;
  //构造函数，直接是TextQuery传进来的3个参数进行初始化
  QueryResult(std::string s,                                       
	      std::shared_ptr<std::set<line_no> > p, 
	      std::shared_ptr<std::vector<std::string> > f)
    : sought(s), lines(p), file(f) {
   
   }
  std::set<line_no>::size_type size() const  {  //获取单词出现过的行数 set<lines_on(行标1...行标n)>的size()操作即数量
    return lines->size(); 
  }
  line_it begin() const {                       //获取set<lines_on>的迭代器
    return lines->begin(); 
  }
  line_it end() const {                         //获取set<lines_on>的迭代器
    return lines->end(); 
  }
  std::shared_ptr<std::vector<std::string> > get_file() {   //获取文件容器
    return file; 
  }
 
 private:
  //保存传入的3个参数
  std::string sought;                               //查询词
  std::shared_ptr<std::set<line_no> > lines;        //行号
  std::shared_ptr<std::vector<std::string> > file;  //文章内容
};

std::ostream &print(std::ostream&, const QueryResult&);
#endif
