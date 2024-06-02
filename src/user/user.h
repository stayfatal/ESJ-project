#ifndef USER_H
#define USER_H

#include <string>
#include "/home/stayfatal/projects/ESJ-project/src/process/process.h"


struct User {
  ProcessEnum process;
  bool isTyping = false;
  int weekPage;
};

#endif