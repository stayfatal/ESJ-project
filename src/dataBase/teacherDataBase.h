#ifndef TEACHER_DATA_BASE_H
#define TEACHER_DATA_BASE_H

#include "dataBase.h"

using std::string;

struct date {
  Day day;
  int hours;
  int minutes;
};

class TeacherDataBase : private DataBase {
 private:
 public:
  string subject;
  string first_name;
  string surname;
  string father_name;
  TeacherDataBase(string subject, string surname);

  bool addDate(date date);
  string showTeacherInformation();
  string showShedule();
};

std::string to_string(const Day& d);

#endif