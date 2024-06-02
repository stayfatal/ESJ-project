#include "homeworkDataBase.h"
#include "dataBase.h"

#include <fstream>

HomeworkDataBase::HomeworkDataBase(string group, int week)
    : DataBase("weekHomework/" + group + "/"), week(week), group(group){};


void HomeworkDataBase::create_path(string last_path){
  path = folder + last_path;
}
void HomeworkDataBase::create_files_structure(string filename){
  fileName = filename;
}

// Функция возвращает текущую неделю
int HomeworkDataBase::getCurrentWeek(boost::posix_time::ptime now) {
  boost::posix_time::ptime semStart(boost::gregorian::date(2024, 2, 5), boost::posix_time::time_duration(0, 0, 0));

  boost::posix_time::time_duration diff = now - semStart;

  int week = static_cast<int>(diff.total_seconds() / 86400 / 7 + 1);

  return week;
}

// Функция возвращает значения поля "неделя" типа int
int HomeworkDataBase::getWeek(std::string str) { return stoi(findField(str, Fields::Week)); }

// Функция форматирует строку с полями и записывает в базу данных
bool HomeworkDataBase::addHomework(std::string line) {
  string weekDay = findField(line, Fields::dayOfWeek);
  bool isValid = false;
  for (int i = static_cast<int>(Day::Monday); i <= static_cast<int>(Day::Sunday); i++) {
    if (weekDay == to_string(static_cast<Day>(i))) {
      isValid = true;
      break;
    }
  }

  addToDb(line + "\n");
  return isValid;
}

std::string HomeworkDataBase::showHomework() {
  std::string result;
  for (int i = static_cast<int>(Day::Monday); i <= static_cast<int>(Day::Sunday); i++) {
    result += to_string(static_cast<Day>(i)) + "\n";
    std::ifstream in(path);
    if (in.is_open()) {
      std::string temp;
      while (getline(in, temp)) {
        if (findField(temp, Fields::dayOfWeek) == to_string(static_cast<Day>(i))) {
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
  boost::posix_time::ptime semStart(boost::gregorian::date(2024, 2, 5), boost::posix_time::time_duration(0, 0, 0));

  boost::posix_time::time_duration diff = now - semStart;

  int dayNum = static_cast<int>(diff.total_seconds() / 86400) % 7 + 1;
  return to_string(static_cast<Day>(dayNum - 1));
}