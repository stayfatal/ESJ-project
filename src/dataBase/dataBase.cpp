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
using std::to_string;
using std::vector;

DataBase::DataBase(string fileName) { // builder pattern
  create_path(folder);
  create_files_structure(fileName);
}

string to_string(const Day& d) {
  switch (d) {
    case Day::Monday:
      return "Понедельник";
    case Day::Tuesday:
      return "Вторник";
    case Day::Wednesday:
      return "Среда";
    case Day::Thursday:
      return "Четверг";
    case Day::Friday:
      return "Пятница";
    case Day::Saturday:
      return "Суббота";
    case Day::Sunday:
      return "Воскресенье";
  }
  return "";
}

// Функция изменения базы данных принимает номер строки, новую строку, и флаг: false дозапись, true перезапись
void DataBase::setLine(int line, string newLine, bool overwrite) {
  std::ifstream in(path);
  vector<string> vec;
  if (in.is_open()) {
    string temp;
    while (std::getline(in, temp)) {
      vec.push_back(temp);
    }
    if (overwrite) {
      vec[line - 1] = newLine;
    } else {
      vec[line - 1] += newLine;
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

string DataBase::setField(string str, Fields fieldNum, string newField) {
  size_t nextIter = 0;
  size_t prevIter = 0;
  int counter = 0;
  while (counter < static_cast<int>(fieldNum)) {
    prevIter = nextIter + 1 * (counter != 0);
    nextIter = str.find(":", nextIter + 1);
    if (nextIter == std::string::npos) {
      nextIter = str.length();
      break;
    }
    counter++;
  }
  return str.replace(prevIter, nextIter - prevIter, newField);
}

void DataBase::setFieldLine(int line, string newField, Fields fieldNum) { setLine(line, setField(getLine(line), fieldNum, newField), true); }

// Функция сравнивает строку со строкой из файла начиная с определенного индекса и определенной длины и возвращает номер этой строки
int DataBase::findLine(int index, int length, string strLine) {
  std::ifstream in(path);
  if (in.is_open()) {
    std::string temp;
    int curLine = 0;
    while (std::getline(in, temp)) {
      curLine++;
      if (temp.substr(index, temp.length() - index) == strLine) {
        in.close();
        return curLine;
      }
    }
    in.close();
  }
  return -1;
}

// Функция сравнивает значение поля с полем определенной нумерации из файла и возвращает номер строки в которой эти поля совпали(:...:-пример поля)
int DataBase::findFieldLine(string fieldValue, Fields fieldNum) {
  std::ifstream in(path);
  if (in.is_open()) {
    string temp;
    int curLine = 0;
    while (std::getline(in, temp)) {
      curLine++;

      if (fieldValue == findField(temp, fieldNum)) {
        in.close();
        return curLine;
      }
    }
    in.close();
  }
  return -1;
}

// Статическая функция возвращает поле по нумерации из строки
string DataBase::findField(string str, Fields fieldNum) {
  size_t nextIter = 0;
  size_t prevIter = 0;
  int counter = 0;
  while (counter < static_cast<int>(fieldNum)) {
    prevIter = nextIter + 1 * (counter != 0);
    nextIter = str.find(":", nextIter + 1);
    if (nextIter == std::string::npos and counter == static_cast<int>(fieldNum) - 1) {
      nextIter = str.length();
      break;
    } else if (nextIter == std::string::npos) {
      return "";
    }
    counter++;
  }
  return str.substr(prevIter, nextIter - prevIter);
}

// Функция возвращает строку из файла по номеру
string DataBase::getLine(int line) {
  std::ifstream in(path);
  if (in.is_open()) {
    string temp;
    for (int i = 0; i < line; i++) {
      getline(in, temp);
    }
    in.close();
    return temp;
  }
  return "";
}

// Добавляет в базу данных строку
void DataBase::addToDb(std::string str) {
  std::ofstream out(path, std::ios::app);
  if (out.is_open()) {
    out << str << std::endl;
    out.close();
  }
}
