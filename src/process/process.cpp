#include "process.h"
#include <vector>

void registation(TgBot::Bot &bot, UserDataBase &userDataBase, std::unordered_map<int64_t, User> &users, HomeworkDataBase &homeWorkBases,
                          TgBot::Message::Ptr message) {
  if (userDataBase.registration(message->text, message->chat->id)) {
    bot.getApi().sendMessage(
        message->chat->id,
        "Вы успешно зарегистрировались в системе, теперь у вас есть возможность пользоваться всеми функциями бота, для этого напишите, /menu");
  } else {
    bot.getApi().sendMessage(message->chat->id, "Ваше ФИО не было найдено в базе, либо ваш пароль неверен");
  }
  users[message->chat->id].isTyping = false;
}

void homework(TgBot::Bot &bot, UserDataBase &userDataBase, std::unordered_map<int64_t, User> &users, HomeworkDataBase &homeWorkBases,
                       TgBot::Message::Ptr message) {
  std::string group = userDataBase.getGroup(message->chat->id);
  int week = HomeworkDataBase::getWeek(message->text);

  homeWorkBases[group][week - 1].addHomework(message->text);
  bot.getApi().sendMessage(message->chat->id, "Ваше домашнее задание успешно записано");

  users[message->chat->id].isTyping = false;
}

void globalMes(TgBot::Bot &bot, UserDataBase &userDataBase, std::unordered_map<int64_t, User> &users, HomeworkDataBase &homeWorkBases,
                        TgBot::Message::Ptr message) {
  std::vector<int64_t> list = userDataBase.getListOfAvalibleUsers("globalMes");
  for (auto i = list.begin(); i != list.end(); i++) {
    if (*i != message->chat->id and *i != 0) {
      bot.getApi().sendMessage(*i, message->text);
    }
  }
  bot.getApi().sendMessage(message->chat->id, "Ваше сообщение было отправлено");
  users[message->chat->id].isTyping = false;
}

typingCommands = {{ProcessEnum::registration, registration}, {ProcessEnum::homework, homework}, {ProcessEnum::globalMes, globalMes}};
