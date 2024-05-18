#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <iostream>
#include <fstream>

using std::string;

class DataBase{
private:
std::string fileName;
public:
DataBase(string fileName);

int findLine(string line);

void editDb(int line,string newLine,bool overwrite=true);
};

#endif