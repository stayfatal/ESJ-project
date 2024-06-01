#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <iostream>

using std::string;


enum class Fields { Rights = 1, Notifications_1, Notifications_2, Group, UserName, Password, UserId, Week = 1, dayOfWeek, Subject, Task };

class DataBase {
 protected:
  string fileName;
  string path;
  string folder = "/home/stayfatal/projects/ESJ-project/data/";

 public:
  DataBase(string fileName);

  int findFieldLine(string str, Fields fieldNum);

  static string findField(string str, Fields fieldNum);

  void addToDb(date date);

  int findLine(int index, int length, string strLine);

  void setLine(int line, string newLine, bool overwrite = true);

  string setField(string str, Fields fieldNum, string newField);

  void setFieldLine(int line, string newField, Fields fieldNum);

  string getLine(int line);

  void addToDb(string str);
};


#endif