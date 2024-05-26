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

void UserDataBase::switchAdvanceNotifications(int64_t chatId, string switchData) {
  int line = findFieldLine(std::to_string(chatId), Fields::UserId);
  setFieldLine(line, switchData, Fields::Notifications_1);
}

void UserDataBase::switchAdminNotifications(int64_t chatId, string switchData) {
  int line = findFieldLine(std::to_string(chatId), Fields::UserId);
  setFieldLine(line, switchData, Fields::Notifications_2);
}

UserDataBase::UserDataBase(string fileName) : DataBase(fileName) {}

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

std::vector<int64_t> UserDataBase::getListOfAvalibleUsers(std::string task) {
  std::vector<int64_t> result;
  if (task == "globalMes") {
    std::ifstream in(path);
    if (in.is_open()) {
      std::string temp;
      while (std::getline(in, temp)) {
        if (findField(temp,Fields::Notifications_2)=="1" and findField(temp,Fields::UserId)!="") {
          result.push_back(stoll(findField(temp,Fields::UserId)));
        }
      }
      in.close();
    }
  }else if(task=="advanceMes"){

  }
  return result;
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
    if (nextIter == std::string::npos and counter == static_cast<int>(fieldNum)-1) {
      nextIter = str.length();
      break;
    }else if(nextIter == std::string::npos){
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

// Функция регистрирует пользователя в базе данных
bool UserDataBase::registration(string name, int64_t chatId) {
  if (findLine(6, name.length(), name) != -1) {
    setLine(findLine(6, name.length(), name), ":" + std::to_string(chatId), false);
    return true;
  } else {
    return false;
  }
}

// Функция проверяет регистрацию
bool UserDataBase::isRegistered(int64_t chatId) {
  if (findFieldLine(std::to_string(chatId), Fields::UserId) != -1) {
    return true;
  } else {
    return false;
  }
}

bool UserDataBase::isAdvanceNotificationsOn(int64_t chatId) {
  int line = findFieldLine(std::to_string(chatId), Fields::UserId);
  return findField(getLine(line), Fields::Notifications_1) == "1" ? true : false;
}

bool UserDataBase::isAdminNotificationsOn(int64_t chatId) {
  int line = findFieldLine(std::to_string(chatId), Fields::UserId);
  return findField(getLine(line), Fields::Notifications_2) == "1" ? true : false;
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
bool UserDataBase::isAdmin(int64_t chatId) {
  int line = findFieldLine(std::to_string(chatId), Fields::UserId);
  if (getLine(line)[0] == 'A') {
    return true;
  }
  return false;
}

// Функция возвращает группу пользователя по его айди
string UserDataBase::getGroup(int64_t chatId) {
  int line = findFieldLine(std::to_string(chatId), Fields::UserId);
  return findField(getLine(line), Fields::Group);
}

// Функция возвращает значения поля "неделя" типа int
int HomeworkDataBase::getWeek(std::string str) { return stoi(findField(str, Fields::Week)); }

// Функция форматирует строку с полями и записывает в базу данных
bool HomeworkDataBase::addHomework(std::string line) {
  std::vector<string> weekDays = {"Понедельник", "Вторник", "Среда", "Черверг", "Пятница", "Суббота", "Воскресенье"};
  string weekDay = findField(line, Fields::dayOfWeek);
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
    result += *i + "\n";
    std::ifstream in(path);
    if (in.is_open()) {
      std::string temp;
      while (getline(in, temp)) {
        if (findField(temp, Fields::dayOfWeek) == *i) {
          result += findField(temp, Fields::Subject) + " -> " + findField(temp, Fields::Task) + "\n";
        }
      }
    }
    in.close();
    result += "\n";
  }
  return result;
}

string HomeworkDataBase::getCurrentDayOfWeek(boost::posix_time::ptime now) {
  std::vector<string> weekDays = {"Понедельник", "Вторник", "Среда", "Черверг", "Пятница", "Суббота", "Воскресенье"};
  boost::posix_time::ptime semStart(boost::gregorian::date(2024, 2, 5), boost::posix_time::time_duration(0, 0, 0));

  boost::posix_time::time_duration diff = now - semStart;

  int dayNum = static_cast<int>(diff.total_seconds() / 86400) % 7 + 1;
  return weekDays[dayNum - 1];
}

TeacherDataBase::TeacherDataBase(string subject, std::string surname)
    : DataBase("teacherShedule/" + subject + "/" + surname + ".txt"), subject(subject), surname(surname){};

bool TeacherDataBase::addDate(date date) {
  addToDb(date);
  return true;
}

void DataBase::addToDb(date Date) {
  std::ofstream out(path, std::ios::app);
  if (out.is_open()) {
    out << "Week day: " << Date.day << " Time: " << Date.hours << ":"  << Date.minutes << std::endl;
    out.close();
  }
}

std::ostream& operator << (std::ostream& out, const Day& d){
   switch(d) {
      case Day::Monday: return (out << "Monday");
      case Day::Tuesday: return (out << "Tuesday");
      case Day::Wednesday:return (out << "Wednesday");
      case Day::Thursday:return (out << "Thursday");
      case Day::Friday:return (out << "Friday");
      case Day::Saturday:return (out << "Saturday");
      case Day::Sunday:return (out << "Sunday");

   }
   return (out);
}

std::string operator += (std::string str, const Day& d){
   switch(d) {
      case Day::Monday: return (str += "Monday");
      case Day::Tuesday: return (str += "Tuesday");
      case Day::Wednesday:return (str += "Wednesday");
      case Day::Thursday:return (str += "Thursday");
      case Day::Friday:return (str += "Friday");
      case Day::Saturday:return (str += "Saturday");
      case Day::Sunday:return (str += "Sunday");

   }
   return (str);
}

bool operator == (std::string str, const Day& d){
   switch(d) {
      case Day::Monday: return (str == "Monday");
      case Day::Tuesday: return (str == "Tuesday");
      case Day::Wednesday:return (str == "Wednesday");
      case Day::Thursday:return (str == "Thursday");
      case Day::Friday:return (str == "Friday");
      case Day::Saturday:return (str == "Saturday");
      case Day::Sunday:return (str == "Sunday");
   }
   return false;
}



std::string TeacherDataBase::showShedule() {
  std::string result;

  for ( int i = 0; i < 7; i++ ){
    Day day = static_cast<Day>(i);

    result+=day;
    result += '\n';
    std::ifstream in(path);
    if (in.is_open()) {
      std::string temp;
      while (getline(in, temp)) {
        // if (findField(temp, 2) == day) {
        //   result += findField(temp, 3) + " -> " + findField(temp, 5) + "\n";
        // }
      }
    }
    in.close();
    result+="\n";

  }
  return result;
}