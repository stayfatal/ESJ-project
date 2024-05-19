#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <iostream>
#include <fstream>

using std::string;

class DataBase{
protected:
std::string fileName;
std::string path;
const std::string folder="/home/stayfatal/projects/ESJ-project/data/";
public:
DataBase(string fileName);

int findLine(string line);

void editDb(int line,string newLine,bool overwrite=true);
};

class RegistrationDataBase : private DataBase{
public:
RegistrationDataBase(string fileName);

int findId(int64_t id);

bool registration(string name, int64_t chatId);

bool isRegistered(int64_t chatId);
};

class HomeWorkDataBase : private DataBase{
public:
    HomeWorkDataBase(string fileName);
};

#endif