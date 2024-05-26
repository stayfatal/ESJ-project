#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <fstream>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using std::string;
enum class Day {Monday = 1, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday};
struct date {

  Day day;
  int hours;
  int minutes;
  
};
std::ostream& operator << (std::ostream& out, const Day& d);



class DataBase {
 protected:
  std::string fileName;
  std::string path;
  std::string folder = "/home/stayfatal/projects/ESJ-project/data/";

 public:
  DataBase(string fileName);

  int findFieldLine(string str, int fieldNum);

  static std::string findField(std::string str, int fieldNum);

  int findLine(int index, int length, string strLine);

  void setLine(int line, string newLine, bool overwrite = true);

  std::string getLine(int line);

  void addToDb(std::string str);
  void addToDb(date date);
};

class RegistrationDataBase : private DataBase {
 public:
  RegistrationDataBase(string fileName);

  bool registration(string name, int64_t chatId);

  bool isRegistered(int64_t chatId);

  std::string getGroup(int64_t chatId);

  bool isAdmin(int64_t chatId);
};

class HomeworkDataBase : private DataBase {
 private:
 public:
  int week;
  std::string group;
  HomeworkDataBase(std::string group, int week);

  bool addHomework(std::string line);
  static int getWeek(std::string str);

  static int getCurrentWeek(boost::posix_time::ptime now);

  std::string showHomework();
};

class TeacherDataBase: private DataBase{
  private:
  public:
    std::string subject;
    std::string first_name;
    std::string surname;
    std::string father_name;
    TeacherDataBase(std::string subject, std::string surname);

    bool addDate(date date);
    std::string showShedule();
    std::string showTeacherInformation(); //почта, номер

//учитывать группу
};

#endif