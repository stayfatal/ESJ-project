#ifndef USER_DATA_BASE_H
#define USER_DATA_BASE_H

#include <vector>

#include "dataBase.h"

enum class Group { Group21 = 1, Group22, Group23, Group24, Group25 };

class UserDataBase : private DataBase {
 public:
  UserDataBase(string fileName);

  bool registration(string name, int64_t chatId);

  bool isRegistered(int64_t chatId);

  string getGroup(int64_t chatId);

  bool isAdmin(int64_t chatId);

  bool isAdvanceNotificationsOn(int64_t chatId);

  void switchAdvanceNotifications(int64_t chatId, string switchData);

  void switchAdminNotifications(int64_t chatId, string switchData);

  bool isAdminNotificationsOn(int64_t chatId);

  std::vector<int64_t> getListOfAvalibleUsers(std::string task);
};

#endif