#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <fstream>
#include <iostream>

using std::string;

class DataBase {
 protected:
  std::string fileName;
  std::string path;
  const std::string folder = "/home/stayfatal/projects/ESJ-project/data/";

 public:
  DataBase(string fileName);

  void editDb(int line, string newLine, bool overwrite = true);
};

class RegistrationDataBase : private DataBase {
 public:
  RegistrationDataBase(string fileName);

  bool registration(string name, int64_t chatId);

  int find(int64_t id, int fieldNum);

  int findLine(string strLine);

  bool isRegistered(int64_t chatId);

  bool isAdmin(int64_t chatId);
};

class HomeWorkDataBase : private DataBase {
 public:
  HomeWorkDataBase(string fileName);

  int getWeek();
};

#endif