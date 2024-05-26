#include <stdio.h>
#include <tgbot/tgbot.h>

#include <cstdio>

#include "dataBase/dataBase.h"

struct User {
  std::string process;
  bool isTyping = false;
  int weekPage;
};

void setCommandsMenu(TgBot::Bot &bot);
TgBot::InlineKeyboardMarkup::Ptr mainMenu();
TgBot::InlineKeyboardMarkup::Ptr homeworkMenu(bool adminAccess);
TgBot::InlineKeyboardMarkup::Ptr showHomeworkMenu();
TgBot::InlineKeyboardMarkup::Ptr notificationsMenu(bool isOn1, bool isOn2, bool adminAccess);
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
                               "Hi!\n I am an EHS (Student's Electronic Journal) – your assistant in studies. \n With my help, you can: \n"
                               "Keep track of current homework and deadlines \n Track your progress \n Receive "
                               "notifications about new assignments and grades \nStart with the /register command to register and start"
                               "to use all my possibilities!\nGood luck with your studies!");
    }
  });

  // Обработка команды register
  bot.getEvents().onCommand("register", [&bot, &userDataBase, &users](TgBot::Message::Ptr message) {
    if (!userDataBase.isRegistered(message->chat->id)) {
      users[message->chat->id].isTyping = true;
      users[message->chat->id].process = "registration";
      bot.getApi().sendMessage(message->chat->id, "Enter your data in the format:\nGroup:Full Name:Password");
    } else {
      bot.getApi().sendMessage(message->chat->id, "You are already registered");
    }
  });

  // Обработка данных который пользователь вводит с клавиатуры
  bot.getEvents().onNonCommandMessage([&bot, &userDataBase, &users, &homeWorkBases](TgBot::Message::Ptr message) {
    if (users[message->chat->id].isTyping && users[message->chat->id].process == "registration") {
      if (userDataBase.registration(message->text, message->chat->id)) {
        bot.getApi().sendMessage(message->chat->id,
                                 "You have successfully registered in the system, now you have the opportunity to use all the functions of the bot, "
                                 "to do this, write /menu");
      } else {
        bot.getApi().sendMessage(message->chat->id, "Your full name was not found in the database, or your password is incorrect");
      }
      users[message->chat->id].isTyping = false;
    } else if (users[message->chat->id].isTyping && users[message->chat->id].process == "homework") {
      std::string group = userDataBase.getGroup(message->chat->id);
      int week = HomeworkDataBase::getWeek(message->text);

      homeWorkBases[group][week - 1].addHomework(message->text);
      bot.getApi().sendMessage(message->chat->id, "Your homework has been successfully recorded");

      users[message->chat->id].isTyping = false;
    } else if (users[message->chat->id].isTyping && users[message->chat->id].process == "globalMes") {
      std::vector<int64_t> list = userDataBase.getListOfAvalibleUsers("globalMes");
      for (auto i = list.begin(); i != list.end(); i++) {
        if (*i != message->chat->id and *i!=0) {
          bot.getApi().sendMessage(*i, message->text);
        }
      }
      bot.getApi().sendMessage(message->chat->id, "Your message was sent");
      users[message->chat->id].isTyping=false;
    }
  });

  // Обработка команды menu
  bot.getEvents().onCommand("menu", [&bot, &userDataBase, &users](TgBot::Message::Ptr message) {
    if (!users[message->chat->id].isTyping) {
      if (userDataBase.isRegistered(message->chat->id)) {
        bot.getApi().sendMessage(message->chat->id, "That's what I can do.", false, 0, mainMenu());
      } else {
        bot.getApi().sendMessage(message->chat->id, "Register to use the bot's features. To register, use the /register command");
      }
    }
  });

  // Обработка команды homework
  bot.getEvents().onCommand("homework", [&bot, &users, &userDataBase](TgBot::Message::Ptr message) {
    if (!users[message->chat->id].isTyping) {
      if (userDataBase.isRegistered(message->chat->id)) {
        bot.getApi().sendMessage(message->chat->id, "Available commands:", false, 0, homeworkMenu(userDataBase.isAdmin(message->chat->id)));
      } else {
        bot.getApi().sendMessage(message->chat->id, "Register to use the bot's features. To register, use the /register command");
      }
    }
  });

  // Обработка нажатий кнопок
  bot.getEvents().onCallbackQuery([&bot, &userDataBase, &users, &homeWorkBases, &currentTime](TgBot::CallbackQuery::Ptr query) {
    if (query->data == "homework") {
      // homework

      bot.getApi().editMessageText("Homework menu:", query->message->chat->id, query->message->messageId, "", "", false,
                                   homeworkMenu(userDataBase.isAdmin(query->message->chat->id)));
    } else if (query->data == "notifications") {
      // notifications
      bot.getApi().editMessageText(
          "Notifications settings:", query->message->chat->id, query->message->messageId, "", "", false,
          notificationsMenu(userDataBase.isAdvanceNotificationsOn(query->message->chat->id),
                            userDataBase.isAdminNotificationsOn(query->message->chat->id), userDataBase.isAdmin(query->message->chat->id)));
    } else if (query->data == "backToMainMenu") {
      // back to main menu
      bot.getApi().editMessageText("That's what I can do:", query->message->chat->id, query->message->messageId, "", "", false, mainMenu());
    } else if (query->data == "exitMenu") {
      bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
    } else if (query->data == "showHomework") {
      // showHomework
      bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
      std::string group = userDataBase.getGroup(query->message->chat->id);
      users[query->message->chat->id].weekPage = HomeworkDataBase::getCurrentWeek(currentTime);
      bot.getApi().sendMessage(query->message->chat->id,
                               std::to_string(users[query->message->chat->id].weekPage) + " week\n" +
                                   homeWorkBases[group][users[query->message->chat->id].weekPage - 1].showHomework(),
                               false, 0, showHomeworkMenu());
    } else if (query->data == "addHomework") {
      // addHomework
      bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
      bot.getApi().sendMessage(query->message->chat->id,
                               "Write me the homework you want to add in the format:\nnumber of the week:Day of the week:Subject:Task");
      users[query->message->chat->id].process = "homework";
      users[query->message->chat->id].isTyping = true;
    } else if (query->data == "prevWeek") {
      // prevWeek
      if (users[query->message->chat->id].weekPage - 1 >= 1) {
        users[query->message->chat->id].weekPage -= 1;
        std::string group = userDataBase.getGroup(query->message->chat->id);
        bot.getApi().editMessageText(std::to_string(users[query->message->chat->id].weekPage) + " week\n" +
                                         homeWorkBases[group][users[query->message->chat->id].weekPage - 1].showHomework(),
                                     query->message->chat->id, query->message->messageId, "", "", false, showHomeworkMenu());
      }
    } else if (query->data == "nextWeek") {
      // nextWeek
      if (users[query->message->chat->id].weekPage + 1 <= 18) {
        users[query->message->chat->id].weekPage += 1;
        std::string group = userDataBase.getGroup(query->message->chat->id);
        bot.getApi().editMessageText(std::to_string(users[query->message->chat->id].weekPage) + " week\n" +
                                         homeWorkBases[group][users[query->message->chat->id].weekPage - 1].showHomework(),
                                     query->message->chat->id, query->message->messageId, "", "", false, showHomeworkMenu());
      }
    } else if (query->data == "advanceNotice") {
      if (userDataBase.isAdvanceNotificationsOn(query->message->chat->id)) {
        userDataBase.switchAdvanceNotifications(query->message->chat->id, "0");
      } else {
        userDataBase.switchAdvanceNotifications(query->message->chat->id, "1");
      }
      bot.getApi().editMessageText(
          "Notifications settings:", query->message->chat->id, query->message->messageId, "", "", false,
          notificationsMenu(userDataBase.isAdvanceNotificationsOn(query->message->chat->id),
                            userDataBase.isAdminNotificationsOn(query->message->chat->id), userDataBase.isAdmin(query->message->chat->id)));
    } else if (query->data == "adminNotice") {
      if (userDataBase.isAdminNotificationsOn(query->message->chat->id)) {
        userDataBase.switchAdminNotifications(query->message->chat->id, "0");
      } else {
        userDataBase.switchAdminNotifications(query->message->chat->id, "1");
      }
      bot.getApi().editMessageText(
          "Notifications settings:", query->message->chat->id, query->message->messageId, "", "", false,
          notificationsMenu(userDataBase.isAdvanceNotificationsOn(query->message->chat->id),
                            userDataBase.isAdminNotificationsOn(query->message->chat->id), userDataBase.isAdmin(query->message->chat->id)));
    } else if (query->data == "globalMessage") {
      users[query->message->chat->id].isTyping = true;
      users[query->message->chat->id].process = "globalMes";
      bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
      bot.getApi().sendMessage(query->message->chat->id, "Text me the message");
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
  startCommand->description = "Start";

  TgBot::BotCommand::Ptr regCommand(new TgBot::BotCommand);
  regCommand->command = "register";
  regCommand->description = "Register";

  TgBot::BotCommand::Ptr menuCommand(new TgBot::BotCommand);
  menuCommand->command = "menu";
  menuCommand->description = "Manu";

  TgBot::BotCommand::Ptr homeworkCommand(new TgBot::BotCommand);
  homeworkCommand->command = "homework";
  homeworkCommand->description = "Homework";

  TgBot::BotCommand::Ptr notifyCommand(new TgBot::BotCommand);
  notifyCommand->command = "notifications";
  notifyCommand->description = "Notifications";

  commands.push_back(startCommand);
  commands.push_back(regCommand);
  commands.push_back(menuCommand);
  commands.push_back(homeworkCommand);
  commands.push_back(notifyCommand);

  bot.getApi().setMyCommands(commands);
}

// Функция возвращающая кнопки основного меню
TgBot::InlineKeyboardMarkup::Ptr mainMenu() {
  TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
  std::vector<TgBot::InlineKeyboardButton::Ptr> row1;
  TgBot::InlineKeyboardButton::Ptr button1(new TgBot::InlineKeyboardButton);
  button1->text = "Homework";
  button1->callbackData = "homework";
  row1.push_back(button1);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
  TgBot::InlineKeyboardButton::Ptr button2(new TgBot::InlineKeyboardButton);
  button2->text = "Notification settins";
  button2->callbackData = "notifications";
  row2.push_back(button2);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row3;
  TgBot::InlineKeyboardButton::Ptr button3(new TgBot::InlineKeyboardButton);
  button3->text = "Exit menu";
  button3->callbackData = "exitMenu";
  row3.push_back(button3);

  keyboard->inlineKeyboard.push_back(row1);
  keyboard->inlineKeyboard.push_back(row2);
  keyboard->inlineKeyboard.push_back(row3);
  return keyboard;
}

// Функция возвращающая кнопки меню домашнего задания
TgBot::InlineKeyboardMarkup::Ptr homeworkMenu(bool adminAccess) {
  TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row1;
  TgBot::InlineKeyboardButton::Ptr button1(new TgBot::InlineKeyboardButton);
  button1->text = "Show homework";
  button1->callbackData = "showHomework";
  row1.push_back(button1);

  keyboard->inlineKeyboard.push_back(row1);

  if (adminAccess) {
    std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
    TgBot::InlineKeyboardButton::Ptr button2(new TgBot::InlineKeyboardButton);
    button2->text = "Add new homework";
    button2->callbackData = "addHomework";
    row2.push_back(button2);

    keyboard->inlineKeyboard.push_back(row2);
  }

  std::vector<TgBot::InlineKeyboardButton::Ptr> row3;
  TgBot::InlineKeyboardButton::Ptr button3(new TgBot::InlineKeyboardButton);
  button3->text = "Menu";
  button3->callbackData = "backToMainMenu";
  row3.push_back(button3);

  keyboard->inlineKeyboard.push_back(row3);

  return keyboard;
}

void configureFiles(std::unordered_map<std::string, std::vector<HomeworkDataBase>> &homeWorkBases) {
  for (auto i = homeWorkBases.begin(); i != homeWorkBases.end(); i++) {
    for (int j = 1; j <= 18; j++) {
      HomeworkDataBase homework(i->first, j);
      i->second.push_back(homework);
    }
  }
}

// Функция возвращающая кнопки меню показа дз
TgBot::InlineKeyboardMarkup::Ptr showHomeworkMenu() {
  TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row1;
  TgBot::InlineKeyboardButton::Ptr button1(new TgBot::InlineKeyboardButton);
  TgBot::InlineKeyboardButton::Ptr button2(new TgBot::InlineKeyboardButton);
  button1->text = "Previous week";
  button1->callbackData = "prevWeek";
  button2->text = "Next week";
  button2->callbackData = "nextWeek";
  row1.push_back(button1);
  row1.push_back(button2);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
  TgBot::InlineKeyboardButton::Ptr button3(new TgBot::InlineKeyboardButton);
  button3->text = "Menu";
  button3->callbackData = "backToMainMenu";
  row2.push_back(button3);

  keyboard->inlineKeyboard.push_back(row1);
  keyboard->inlineKeyboard.push_back(row2);

  return keyboard;
}

TgBot::InlineKeyboardMarkup::Ptr notificationsMenu(bool isOn1, bool isOn2, bool adminAccess) {
  TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row1;
  TgBot::InlineKeyboardButton::Ptr button1(new TgBot::InlineKeyboardButton);
  std::string isOnStr1 = (isOn1 ? "ON" : "OFF");
  button1->text = "Advance notice of homework submission -> " + isOnStr1;
  button1->callbackData = "advanceNotice";
  row1.push_back(button1);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
  TgBot::InlineKeyboardButton::Ptr button2(new TgBot::InlineKeyboardButton);
  std::string isOnStr2 = (isOn2 ? "ON" : "OFF");
  button2->text = "Notifications from admins -> " + isOnStr2;
  button2->callbackData = "adminNotice";
  row2.push_back(button2);

  if (adminAccess) {
    std::vector<TgBot::InlineKeyboardButton::Ptr> row3;
    TgBot::InlineKeyboardButton::Ptr button3(new TgBot::InlineKeyboardButton);
    button3->text = "Send message to your group";
    button3->callbackData = "globalMessage";
    row3.push_back(button3);

    keyboard->inlineKeyboard.push_back(row3);
  }

  std::vector<TgBot::InlineKeyboardButton::Ptr> row4;
  TgBot::InlineKeyboardButton::Ptr button4(new TgBot::InlineKeyboardButton);
  button4->text = "Menu";
  button4->callbackData = "backToMainMenu";
  row4.push_back(button4);

  keyboard->inlineKeyboard.push_back(row1);
  keyboard->inlineKeyboard.push_back(row2);
  keyboard->inlineKeyboard.push_back(row4);

  return keyboard;
}

void refreshCurrentTime(boost::posix_time::ptime &currentTime) { currentTime = boost::posix_time::second_clock::local_time(); }

int getHours(boost::posix_time::ptime &currentTime) {
  boost::posix_time::time_duration timeOfDay = currentTime.time_of_day();

  int hours = timeOfDay.hours();
  return hours;
}