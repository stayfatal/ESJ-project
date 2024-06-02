#ifndef COMMANDS_H
#define COMMANDS_H

#include <tgbot/tgbot.h>

#include "/home/stayfatal/projects/ESJ-project/src/dataBase/homeworkDataBase.h"
#include "/home/stayfatal/projects/ESJ-project/src/dataBase/teacherDataBase.h"
#include "/home/stayfatal/projects/ESJ-project/src/dataBase/userDataBase.h"
#include "/home/stayfatal/projects/ESJ-project/src/keybords/keybords.h"
#include "/home/stayfatal/projects/ESJ-project/src/user/user.h"
#include "unordered_map"
#include "/home/stayfatal/projects/ESJ-project/src/process/process.h"

void handleStartCommand(TgBot::Bot &bot, std::unordered_map<int64_t, User> &users, TgBot::Message::Ptr message);

void handleRegisterCommand(TgBot::Bot &bot, UserDataBase &userDataBase, std::unordered_map<int64_t, User> &users, TgBot::Message::Ptr message);

void handleMenuCommand(TgBot::Bot &bot, UserDataBase &userDataBase, std::unordered_map<int64_t, User> &users, TgBot::Message::Ptr message);

void handleCancelCommand(TgBot::Bot &bot, UserDataBase &userDataBase, std::unordered_map<int64_t, User> &users, TgBot::Message::Ptr message);

#endif