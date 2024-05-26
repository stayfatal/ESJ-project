#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>
#include <iostream>

using std::string;
enum class Day { Monday = 1, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday };
struct date {
  Day day;
  int hours;
  int minutes;
};

std::ostream& operator << (std::ostream& out, const Day& d);

enum class Fields { Rights = 1, Notifications_1, Notifications_2, Group, UserName, Password, UserId, Week = 1, dayOfWeek, Subject, Task };

class DataBase {
 protected:
  string fileName;
  string path;
  string folder = "/home/stayfatal/projects/ESJ-project/data/";

 public:
  DataBase(string fileName);

  int findFieldLine(string str, Fields fieldNum);

  static string findField(string str, Fields fieldNum);

  void addToDb(date date);

  int findLine(int index, int length, string strLine);

  void setLine(int line, string newLine, bool overwrite = true);

  string setField(string str, Fields fieldNum, string newField);

  void setFieldLine(int line, string newField, Fields fieldNum);

  string getLine(int line);

  void addToDb(string str);
};

class UserDataBase : private DataBase {
 public:
  UserDataBase(string fileName);

  bool registration(string name, int64_t chatId);

  bool isRegistered(int64_t chatId);

  string getGroup(int64_t chatId);

  bool isAdmin(int64_t chatId);

  bool isAdvanceNotificationsOn(int64_t chatId);

  void switchAdvanceNotifications(int64_t chatId,string switchData);

  void switchAdminNotifications(int64_t chatId,string switchData);

  bool isAdminNotificationsOn(int64_t chatId);

  std::vector<int64_t> getListOfAvalibleUsers(std::string task);
};

class HomeworkDataBase : private DataBase {
 private:
 public:
  int week;
  string group;
  HomeworkDataBase(string group, int week);

  bool addHomework(string line);
  static int getWeek(string str);

  static int getCurrentWeek(boost::posix_time::ptime now);
  static string getCurrentDayOfWeek(boost::posix_time::ptime now);

  string showHomework();
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

#endif