#include <stdio.h>
#include <tgbot/tgbot.h>

#include <cstdio>

#include "dataBase/dataBase.h"
#include "dataBase/homeworkDataBase.h"
#include "dataBase/teacherDataBase.h"
#include "dataBase/userDataBase.h"

#include "keybord/keybord.h"

struct User {
  std::string process;
  bool isTyping = false;
  int weekPage;
};

void setCommandsMenu(TgBot::Bot &bot);

void configureFiles(std::unordered_map<std::string, std::vector<HomeworkDataBase>> &homeWorkBases);
void refreshCurrentTime(boost::posix_time::ptime &currentTime);
int getHours(boost::posix_time::ptime &currentTime);

int main() {
  TgBot::Bot bot("6754420400:AAFFvP4JGhKlgnRVCQrERYe096WliGeg5yg");
  UserDataBase userDataBase("userDataBase.txt");
  std::vector<HomeworkDataBase> Group25;
  std::vector<HomeworkDataBase> Group24;
  std::unordered_map<std::string, std::vector<HomeworkDataBase>> homeWorkBases{{"ИУ5-25Б", Group25}, {"ИУ5-24Б", Group24}};
  std::unordered_map<int64_t, User> users;

  boost::posix_time::ptime currentTime;

  setCommandsMenu(bot);  // Установка меню с командами
  configureFiles(homeWorkBases);

  // Обработка команды старт
  bot.getEvents().onCommand("start", [&bot, &userDataBase, &users](TgBot::Message::Ptr message) {
    if (!users[message->chat->id].isTyping) {
      bot.getApi().sendMessage(message->chat->id,
                               "Привет! \n👋 Я ЭЖС (Электронный Журнал Студента) – ваш помощник в учебе. \n📚 С моей помощью вы можете: \n✅ "
                               "Следить за текущими домашними заданиями и дедлайнами \n✅ Отслеживать свою успеваемость \n✅ Получать "
                               "уведомления о новых заданиях и оценках \nНачните с команды /register, чтобы пройти регистрацию и начать "
                               "использовать все мои возможности!\nУдачи в учебе! 🚀");
    }
  });

  // Обработка команды register
  bot.getEvents().onCommand("register", [&bot, &userDataBase, &users](TgBot::Message::Ptr message) {
    if (!userDataBase.isRegistered(message->chat->id)) {
      users[message->chat->id].isTyping = true;
      users[message->chat->id].process = "registration";
      bot.getApi().sendMessage(message->chat->id,
                               "Введите ваши данные в формате:\nГруппа:ФИО:Пароль\nex: ИУ7-15Б:Иванов Иван Иванович:123\n/cancel для отмены");
    } else {
      bot.getApi().sendMessage(message->chat->id, "Вы уже зарегистрированы");
    }
  });

  // Обработка команды menu
  bot.getEvents().onCommand("menu", [&bot, &userDataBase, &users](TgBot::Message::Ptr message) {
    if (!users[message->chat->id].isTyping) {
      if (userDataBase.isRegistered(message->chat->id)) {
        bot.getApi().sendMessage(message->chat->id, "Вот что я умею", false, 0, mainMenu());
      } else {
        bot.getApi().sendMessage(message->chat->id,
                                 "Зарегистрируйтесь чтобы использовать функции бота. Для регистрации используйте команду /register");
      }
    }
  });

  // Обработка команды cancel
  bot.getEvents().onCommand("cancel", [&bot, &userDataBase, &users](TgBot::Message::Ptr message) {
    if (users[message->chat->id].isTyping) {
      users[message->chat->id].isTyping = false;
      bot.getApi().sendMessage(message->chat->id, "Операция отменена",false,0,returnToMenu());
    }
  });

  // Обработка данных который пользователь вводит с клавиатуры
  bot.getEvents().onNonCommandMessage([&bot, &userDataBase, &users, &homeWorkBases](TgBot::Message::Ptr message) {
    if (users[message->chat->id].isTyping && users[message->chat->id].process == "registration") {
      if (userDataBase.registration(message->text, message->chat->id)) {
        bot.getApi().sendMessage(
            message->chat->id,
            "Вы успешно зарегистрировались в системе, теперь у вас есть возможность пользоваться всеми функциями бота, для этого напишите, /menu");
      } else {
        bot.getApi().sendMessage(message->chat->id, "Ваше ФИО не было найдено в базе, либо ваш пароль неверен");
      }
      users[message->chat->id].isTyping = false;
      return;
    } else if (users[message->chat->id].isTyping && users[message->chat->id].process == "homework") {
      std::string group = userDataBase.getGroup(message->chat->id);
      int week = HomeworkDataBase::getWeek(message->text);

      homeWorkBases[group][week - 1].addHomework(message->text);
      bot.getApi().sendMessage(message->chat->id, "Ваше домашнее задание успешно записано");

      users[message->chat->id].isTyping = false;
      return;
    } else if (users[message->chat->id].isTyping && users[message->chat->id].process == "globalMes") {
      std::vector<int64_t> list = userDataBase.getListOfAvalibleUsers("globalMes");
      for (auto i = list.begin(); i != list.end(); i++) {
        if (*i != message->chat->id and *i != 0) {
          bot.getApi().sendMessage(*i, message->text);
        }
      }
      bot.getApi().sendMessage(message->chat->id, "Ваше сообщение было отправлено");
      users[message->chat->id].isTyping = false;
      return;
    }
  });

  // Обработка нажатий кнопок
  bot.getEvents().onCallbackQuery([&bot, &userDataBase, &users, &homeWorkBases, &currentTime](TgBot::CallbackQuery::Ptr query) {
    if (query->data == "homework") {
      // homework

      bot.getApi().editMessageText("Меню домашки:", query->message->chat->id, query->message->messageId, "", "", false,
                                   homeworkMenu(userDataBase.isAdmin(query->message->chat->id)));
    } else if (query->data == "notifications") {
      // notifications
      bot.getApi().editMessageText(
          "Найстройки уведомлений:", query->message->chat->id, query->message->messageId, "", "", false,
          notificationsMenu(userDataBase.isAdvanceNotificationsOn(query->message->chat->id),
                            userDataBase.isAdminNotificationsOn(query->message->chat->id), userDataBase.isAdmin(query->message->chat->id)));
    } else if (query->data == "backToMainMenu") {
      // back to main menu
      bot.getApi().editMessageText("Вот что я умею:", query->message->chat->id, query->message->messageId, "", "", false, mainMenu());
    } else if (query->data == "exitMenu") {
      bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
    } else if (query->data == "showHomework") {
      // showHomework
      bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
      std::string group = userDataBase.getGroup(query->message->chat->id);
      users[query->message->chat->id].weekPage = HomeworkDataBase::getCurrentWeek(currentTime);
      bot.getApi().sendMessage(query->message->chat->id,
                               std::to_string(users[query->message->chat->id].weekPage) + " неделя\n" +
                                   homeWorkBases[group][users[query->message->chat->id].weekPage - 1].showHomework(),
                               false, 0, showHomeworkMenu());
    } else if (query->data == "addHomework") {
      // addHomework
      bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
      bot.getApi().sendMessage(
          query->message->chat->id,
          "Нашишите мне домашнее задание которое хотите добавить в формате:\nНомер недели:День недели:Предмет:Задание\n/cancel для отмены");
      users[query->message->chat->id].process = "homework";
      users[query->message->chat->id].isTyping = true;
    } else if (query->data == "prevWeek") {
      // prevWeek
      if (users[query->message->chat->id].weekPage - 1 >= 1) {
        users[query->message->chat->id].weekPage -= 1;
        std::string group = userDataBase.getGroup(query->message->chat->id);
        bot.getApi().editMessageText(std::to_string(users[query->message->chat->id].weekPage) + " неделя\n" +
                                         homeWorkBases[group][users[query->message->chat->id].weekPage - 1].showHomework(),
                                     query->message->chat->id, query->message->messageId, "", "", false, showHomeworkMenu());
      }
    } else if (query->data == "nextWeek") {
      // nextWeek
      if (users[query->message->chat->id].weekPage + 1 <= 18) {
        users[query->message->chat->id].weekPage += 1;
        std::string group = userDataBase.getGroup(query->message->chat->id);
        bot.getApi().editMessageText(std::to_string(users[query->message->chat->id].weekPage) + " неделя\n" +
                                         homeWorkBases[group][users[query->message->chat->id].weekPage - 1].showHomework(),
                                     query->message->chat->id, query->message->messageId, "", "", false, showHomeworkMenu());
      }
    } else if (query->data == "advanceNotice") {
      // advanceNotice
      if (userDataBase.isAdvanceNotificationsOn(query->message->chat->id)) {
        userDataBase.switchAdvanceNotifications(query->message->chat->id, "0");
      } else {
        userDataBase.switchAdvanceNotifications(query->message->chat->id, "1");
      }
      bot.getApi().editMessageText(
          "Найстройки уведомлений:", query->message->chat->id, query->message->messageId, "", "", false,
          notificationsMenu(userDataBase.isAdvanceNotificationsOn(query->message->chat->id),
                            userDataBase.isAdminNotificationsOn(query->message->chat->id), userDataBase.isAdmin(query->message->chat->id)));
    } else if (query->data == "adminNotice") {
      // adminNotice
      if (userDataBase.isAdminNotificationsOn(query->message->chat->id)) {
        userDataBase.switchAdminNotifications(query->message->chat->id, "0");
      } else {
        userDataBase.switchAdminNotifications(query->message->chat->id, "1");
      }
      bot.getApi().editMessageText(
          "Найстройки уведомлений:", query->message->chat->id, query->message->messageId, "", "", false,
          notificationsMenu(userDataBase.isAdvanceNotificationsOn(query->message->chat->id),
                            userDataBase.isAdminNotificationsOn(query->message->chat->id), userDataBase.isAdmin(query->message->chat->id)));
    } else if (query->data == "globalMessage") {
      // globalMessage
      users[query->message->chat->id].isTyping = true;
      users[query->message->chat->id].process = "globalMes";
      bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
      bot.getApi().sendMessage(query->message->chat->id, "Напиши мне текст сообщения\n/cancel для отмены");
    }
  });

  try {
    printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
    TgBot::TgLongPoll longPoll(bot);
    while (true) {
      refreshCurrentTime(currentTime);
      printf("Long poll started\n");
      longPoll.start();
    }
  } catch (TgBot::TgException &e) {
    printf("error: %s\n", e.what());
  }
  return 0;
}

// Функция которая создает меню со всеми командами
void setCommandsMenu(TgBot::Bot &bot) {
  std::vector<TgBot::BotCommand::Ptr> commands;

  TgBot::BotCommand::Ptr startCommand(new TgBot::BotCommand);
  startCommand->command = "start";
  startCommand->description = "Начать";

  TgBot::BotCommand::Ptr regCommand(new TgBot::BotCommand);
  regCommand->command = "register";
  regCommand->description = "Зарегестрироваться";

  TgBot::BotCommand::Ptr menuCommand(new TgBot::BotCommand);
  menuCommand->command = "menu";
  menuCommand->description = "Меню";

  commands.push_back(startCommand);
  commands.push_back(regCommand);
  commands.push_back(menuCommand);

  bot.getApi().setMyCommands(commands);
}

void configureFiles(std::unordered_map<std::string, std::vector<HomeworkDataBase>> &homeWorkBases) {
  for (auto i = homeWorkBases.begin(); i != homeWorkBases.end(); i++) {
    for (int j = 1; j <= 18; j++) {
      HomeworkDataBase homework(i->first, j);
      i->second.push_back(homework);
    }
  }
}

void refreshCurrentTime(boost::posix_time::ptime &currentTime) { currentTime = boost::posix_time::second_clock::local_time(); }

int getHours(boost::posix_time::ptime &currentTime) {
  boost::posix_time::time_duration timeOfDay = currentTime.time_of_day();

  int hours = timeOfDay.hours();
  return hours;
}