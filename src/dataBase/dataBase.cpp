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

// Функция сравнивает строку со строкой из файла начиная с определенного индекса и определенной длины и возвращает номер этой строки
int DataBase::findLine(int index, int length, string strLine) {
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

// Функция сравнивает значение поля с полем определенной нумерации из файла и возвращает номер строки в которой эти поля совпали(:...:-пример поля)
int DataBase::findFieldLine(string fieldValue, int fieldNum) {
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
string DataBase::findField(string str, int fieldNum) {
  size_t nextIter = 0;
  size_t prevIter = 0;
  int counter = 0;
  while (counter < fieldNum) {
    prevIter = nextIter + 1 * (counter != 0);
    nextIter = str.find(":", nextIter + 1);
    if (nextIter == std::string::npos) {
      nextIter = str.length();
      break;
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

// Функция регистрирует пользователя в базе данных
bool RegistrationDataBase::registration(string name, int64_t chatId) {
  if (findLine(2, name.length(), name) != -1) {
    setLine(findLine(2, name.length(), name) - 1, ":" + std::to_string(chatId), false);
    return true;
  } else {
    return false;
  }
}

// Функция проверяет регистрацию
bool RegistrationDataBase::isRegistered(int64_t chatId) {
  if (findFieldLine(std::to_string(chatId), 5) != -1) {
    return true;
  } else {
    return false;
  }
}

HomeworkDataBase::HomeworkDataBase(string group, int week)
    : DataBase("weekHomework/" + group + "/" + std::to_string(week) + ".txt"), week(week), group(group){};

// Функция возвращает текущую неделю
int HomeworkDataBase::getCurrentWeek(boost::posix_time::ptime now) {
  boost::posix_time::ptime semStart(boost::gregorian::date(2024, 2, 5), boost::posix_time::time_duration(0, 0, 0));

  boost::posix_time::time_duration diff = now - semStart;

  int week = static_cast<int>(diff.total_seconds() / 86400 / 7 + 1);

  return week;
}

// Функция проверяет является ли пользователь админом
bool RegistrationDataBase::isAdmin(int64_t chatId) {
  int line = findFieldLine(std::to_string(chatId), 5);
  if (getLine(line)[0] == 'A') {
    return true;
  }
  return false;
}

// Функция возвращает группу пользователя по его айди
string RegistrationDataBase::getGroup(int64_t chatId) {
  int line = findFieldLine(std::to_string(chatId), 5);
  return findField(getLine(line), 2);
}

// Функция возвращает значения поля "неделя" типа int
int HomeworkDataBase::getWeek(std::string str) { return stoi(findField(str, 1)); }

// Функция форматирует строку с полями и записывает в базу данных
bool HomeworkDataBase::addHomework(std::string line) {
  std::vector<string> weekDays = {"Понедельник", "Вторник", "Среда", "Черверг", "Пятница", "Суббота", "Воскресенье"};
  string weekDay = findField(line, 2);
  for (auto i = weekDays.begin(); i != weekDays.end(); i++) {
    if (*i == weekDay) {
      break;
    } else if (i == weekDays.end()) {
      return false;
    }
  }

  addToDb(line + "\n");
  return true;
}

std::string HomeworkDataBase::showHomework() {
  std::vector<string> weekDays = {"Понедельник", "Вторник", "Среда", "Черверг", "Пятница", "Суббота", "Воскресенье"};
  std::string result;
  for (auto i = weekDays.begin(); i != weekDays.end(); i++) {
    result+=*i+"\n";
    std::ifstream in(path);
    if (in.is_open()) {
      std::string temp;
      while (getline(in, temp)) {
        if (findField(temp, 2) == *i) {
          result += findField(temp, 3) + " -> " + findField(temp, 4) + "\n";
        }
      }
    }
    in.close();
    result+="\n";
  }
  return result;
}

TeacherDataBase::TeacherDataBase(string subject, std::string surname)
    : DataBase("teacherShedule/" + subject + "/" + surname + ".txt"), subject(subject), surname(surname){};

bool TeacherDataBase::addDate(date date){
  
  //addToDb(date);
  return true;
}