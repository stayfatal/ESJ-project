#ifndef PROCESS_H
#define PROCESS_H

#include <tgbot/tgbot.h>

#include "/home/stayfatal/projects/ESJ-project/src/dataBase/homeworkDataBase.h"
#include "/home/stayfatal/projects/ESJ-project/src/dataBase/teacherDataBase.h"
#include "/home/stayfatal/projects/ESJ-project/src/dataBase/userDataBase.h"
#include "/home/stayfatal/projects/ESJ-project/src/keybords/keybords.h"
#include "/home/stayfatal/projects/ESJ-project/src/user/user.h"
#include "unordered_map"


enum class ProcessEnum { registration, homework, globalMes };

void registation(TgBot::Bot &bot, UserDataBase &userDataBase, std::unordered_map<int64_t, User> &users, HomeworkDataBase &homeWorkBases,
                 TgBot::Message::Ptr message);

void homework(TgBot::Bot &bot, UserDataBase &userDataBase, std::unordered_map<int64_t, User> &users, HomeworkDataBase &homeWorkBases,
              TgBot::Message::Ptr message);

void globalMes(TgBot::Bot &bot, UserDataBase &userDataBase, std::unordered_map<int64_t, User> &users, HomeworkDataBase &homeWorkBases,
               TgBot::Message::Ptr message);

std::unordered_map<ProcessEnum, void (*)(TgBot::Bot &bot, UserDataBase &userDataBase, std::unordered_map<int64_t, User> &users,
                                         HomeworkDataBase &homeWorkBases, TgBot::Message::Ptr message)>
    typingCommands;


#endif