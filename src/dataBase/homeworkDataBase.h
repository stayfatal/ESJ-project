#ifndef HOMEWORK_DATA_BASE_H
#define HOMEWORK_DATA_BASE_H

#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "dataBase.h"

class HomeworkDataBase : private DataBase {
 private:
 public:
  int week;
  string group;
  HomeworkDataBase(string group, int week);
  void create_path(string) override;
  void create_files_structure(string) override;
  bool addHomework(string line);
  static int getWeek(string str);

  static int getCurrentWeek(boost::posix_time::ptime now);
  static string getCurrentDayOfWeek(boost::posix_time::ptime now);

  string showHomework();
};

#endif