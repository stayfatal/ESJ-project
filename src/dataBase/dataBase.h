#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <fstream>
#include <iostream>

using std::string;

class DataBase {
 protected:
  std::string fileName;
  std::string path;
  std::string folder = "/home/stayfatal/projects/ESJ-project/data/";

 public:
  DataBase(string fileName);

  int findFieldLine(string str, int fieldNum);

  static std::string findField(std::string str, int fieldNum);

  int findLine(int index, int length, string strLine);

  void setLine(int line, string newLine, bool overwrite = true);

  std::string getLine(int line);

  void addToDb(std::string str);
};

class RegistrationDataBase : private DataBase {
 public:
  RegistrationDataBase(string fileName);

  bool registration(string name, int64_t chatId);

  bool isRegistered(int64_t chatId);

  std::string getGroup(int64_t chatId);

  bool isAdmin(int64_t chatId);
};

class HomeworkDataBase : private DataBase {
 private:
 public:
  int week;
  std::string group;
  HomeworkDataBase(std::string group, int week);

  void addToDbWithFormatting(std::string line);
  static int getWeek(std::string str);

  static int getCurrentWeek();
};

#endif