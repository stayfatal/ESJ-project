#include "dataBase.h"

#include <stdio.h>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

using std::string;
using std::vector;

DataBase::DataBase(string fileName) {
  this->fileName = fileName;
  this->path = folder + fileName;
}

void DataBase::editDb(int line, string newLine, bool overwrite) {
  std::ifstream in(path);
  vector<string> vec;
  if (in.is_open()) {
    string temp;
    while (std::getline(in, temp)) {
      vec.push_back(temp);
    }
    if (overwrite) {
      vec[line] = newLine;
    } else {
      vec[line] += newLine;
    }
    in.close();
  }

  std::ofstream out(path);
  if (out.is_open()) {
    for (auto i : vec) {
      out << i << std::endl;
    }
    out.close();
  }
}

RegistrationDataBase::RegistrationDataBase(string fileName) : DataBase(fileName) {}

int RegistrationDataBase::findLine(string strLine) {
  std::ifstream in(path);
  if (in.is_open()) {
    std::string temp;
    int curLine = 0;
    while (std::getline(in, temp)) {
      curLine++;
      if (temp.substr(2, temp.length() - 2) == strLine) {
        in.close();
        return curLine;
      }
    }
    in.close();
  }
  return -1;
}

int RegistrationDataBase::find(int64_t id, int fieldNum) {
  std::ifstream in(path);
  if (in.is_open()) {
    string temp;
    string strId = std::to_string(id);
    int curLine = 0;
    while (std::getline(in, temp)) {
      curLine++;
      size_t nextIter = 0;
      size_t prevIter = 0;
      int counter = 0;
      while (counter < fieldNum) {
        prevIter = nextIter;
        nextIter = temp.find(":", nextIter + 1);
        if (nextIter == std::string::npos) {
          nextIter = temp.length();
          break;
        }
        counter++;
      }
      if (strId == temp.substr(prevIter + 1, nextIter)) {
        in.close();
        return curLine;
      }
    }
    in.close();
  }
  return -1;
}

bool RegistrationDataBase::registration(string name, int64_t chatId) {
  if (findLine(name) != -1) {
    editDb(findLine(name) - 1, ":" + std::to_string(chatId), false);
    return true;
  } else {
    return false;
  }
}

bool RegistrationDataBase::isRegistered(int64_t chatId) {
  if (find(chatId, 5) != -1) {
    return true;
  } else {
    return false;
  }
}

HomeWorkDataBase::HomeWorkDataBase(string fileName) : DataBase(fileName){};

int HomeWorkDataBase::getWeek() {
  boost::asio::io_context io_context;

  boost::posix_time::ptime now = boost::posix_time::second_clock::universal_time();

  boost::posix_time::ptime semStart(boost::gregorian::date(2024, 2, 5), boost::posix_time::time_duration(0, 0, 0));

  boost::posix_time::time_duration diff = now - semStart;

  int week = static_cast<int>(diff.total_seconds() / 86400 / 7 + 1);

  io_context.run();

  return week;
}

bool RegistrationDataBase::isAdmin(int64_t chatId) {
  int line = find(chatId, 5);
  std::ifstream in(path);
  if (in.is_open()) {
    string temp;
    for (int i = 0; i < line; i++) {
      getline(in, temp);
    }
    if (temp[0] == 'A') {
      return true;
    }
    in.close();
  }
  return false;
}
