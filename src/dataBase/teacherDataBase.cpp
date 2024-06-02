#include "teacherDataBase.h"
#include "dataBase.h"

#include <fstream>

TeacherDataBase::TeacherDataBase(string subject, std::string surname)
    : DataBase("teacherShedule/" + subject + "/" + surname + ".txt"), subject(subject), surname(surname){};

bool TeacherDataBase::addDate(date date) {
  addToDb(date);
  return true;
}

void DataBase::addToDb(date Date) {
  std::ofstream out(path, std::ios::app);
  if (out.is_open()) {
    out << "Week day: " << to_string(Date.day) << " Time: " << Date.hours << ":" << Date.minutes << std::endl;
    out.close();
  }
}

void TeacherDataBase::create_path(string last_path){
  path = folder + last_path;
}
void TeacherDataBase::create_files_structure(string filename){
  fileName = filename;
}


std::string TeacherDataBase::showShedule() {
  std::string result;

  for (int i = 0; i < 7; i++) {
    Day day = static_cast<Day>(i);

    result += to_string(day);
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
    result += "\n";
  }
  return result;
}