#include "userDataBase.h"
#include <vector>
#include <fstream>

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

void UserDataBase::create_path(string last_path){
  path = folder + last_path;
}
void UserDataBase::create_files_structure(string filename){
  fileName = filename;
}

void UserDataBase::switchAdvanceNotifications(int64_t chatId, string switchData) {
  int line = findFieldLine(std::to_string(chatId), Fields::UserId);
  setFieldLine(line, switchData, Fields::Notifications_1);
}

void UserDataBase::switchAdminNotifications(int64_t chatId, string switchData) {
  int line = findFieldLine(std::to_string(chatId), Fields::UserId);
  setFieldLine(line, switchData, Fields::Notifications_2);
}

UserDataBase::UserDataBase(string fileName) : DataBase(fileName) {}

std::vector<int64_t> UserDataBase::getListOfAvalibleUsers(std::string task) {
  std::vector<int64_t> result;
  if (task == "globalMes") {
    std::ifstream in(path);
    if (in.is_open()) {
      std::string temp;
      while (std::getline(in, temp)) {
        if (findField(temp, Fields::Notifications_2) == "1" and findField(temp, Fields::UserId) != "") {
          result.push_back(stoll(findField(temp, Fields::UserId)));
        }
      }
      in.close();
    }
  } else if (task == "advanceMes") {
  }
  return result;
}

bool UserDataBase::isAdvanceNotificationsOn(int64_t chatId) {
  int line = findFieldLine(std::to_string(chatId), Fields::UserId);
  return findField(getLine(line), Fields::Notifications_1) == "1" ? true : false;
}

bool UserDataBase::isAdminNotificationsOn(int64_t chatId) {
  int line = findFieldLine(std::to_string(chatId), Fields::UserId);
  return findField(getLine(line), Fields::Notifications_2) == "1" ? true : false;
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