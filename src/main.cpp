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
TgBot::InlineKeyboardMarkup::Ptr noteMenu();
TgBot::InlineKeyboardMarkup::Ptr showHomeworkMenu();
void configureFiles(std::unordered_map<std::string, std::vector<HomeworkDataBase>> &homeWorkBases);
void refreshCurrentTime(boost::posix_time::ptime &currentTime);

int main() {
  TgBot::Bot bot("6754420400:AAFFvP4JGhKlgnRVCQrERYe096WliGeg5yg");
  RegistrationDataBase registrationDataBase("registrationDataBase.txt");
  std::vector<HomeworkDataBase> Group25;
  std::vector<HomeworkDataBase> Group24;
  std::unordered_map<std::string, std::vector<HomeworkDataBase>> homeWorkBases{{"ИУ5-25Б", Group25}, {"ИУ5-24Б", Group24}};
  std::unordered_map<int64_t, User> users;

  boost::posix_time::ptime currentTime;

  setCommandsMenu(bot);  // Установка меню с командами
  configureFiles(homeWorkBases);

  // Обработка команды старт
  bot.getEvents().onCommand("start", [&bot, &registrationDataBase, &users](TgBot::Message::Ptr message) {
    if (!users[message->chat->id].isTyping) {
      bot.getApi().sendMessage(message->chat->id,
                               "Привет! \n👋 Я ЭЖС (Электронный Журнал Студента) – ваш помощник в учебе. \n📚 С моей помощью вы можете: \n✅ "
                               "Следить за текущими домашними заданиями и дедлайнами \n✅ Отслеживать свою успеваемость \n✅ Получать "
                               "уведомления о новых заданиях и оценках \nНачните с команды /register, чтобы пройти регистрацию и начать "
                               "использовать все мои возможности!\nУдачи в учебе! 🚀");
    }
  });

  // Обработка команды register
  bot.getEvents().onCommand("register", [&bot, &registrationDataBase, &users](TgBot::Message::Ptr message) {
    if (!registrationDataBase.isRegistered(message->chat->id)) {
      users[message->chat->id].isTyping = true;
      users[message->chat->id].process = "registration";
      bot.getApi().sendMessage(message->chat->id, "Введите ваши данные в формате:\nГруппа:ФИО:Пароль");
    } else {
      bot.getApi().sendMessage(message->chat->id, "Вы уже зарегистрированы");
    }
  });

  // Обработка данных который пользователь вводит с клавиатуры
  bot.getEvents().onNonCommandMessage([&bot, &registrationDataBase, &users, &homeWorkBases](TgBot::Message::Ptr message) {
    if (users[message->chat->id].isTyping and users[message->chat->id].process == "registration") {
      if (registrationDataBase.registration(message->text, message->chat->id)) {
        bot.getApi().sendMessage(
            message->chat->id,
            "Вы успешно зарегистрировались в системе, теперь у вас есть возможность пользоваться всеми функциями бота, для этого напишите /menu");
      } else {
        bot.getApi().sendMessage(message->chat->id, "Ваше ФИО не было найдено в базе, либо ваш пароль неверен");
      }
      users[message->chat->id].isTyping = false;
    } else if (users[message->chat->id].isTyping and users[message->chat->id].process == "homework") {
      std::string group = registrationDataBase.getGroup(message->chat->id);
      int week = HomeworkDataBase::getWeek(message->text);

      homeWorkBases[group][week - 1].addHomework(message->text);
      bot.getApi().sendMessage(message->chat->id, "Ваше домашнее задание успешно записано");

      users[message->chat->id].isTyping = false;
    }
  });

  // Обработка команды menu
  bot.getEvents().onCommand("menu", [&bot, &registrationDataBase, &users](TgBot::Message::Ptr message) {
    if (!users[message->chat->id].isTyping) {
      if (registrationDataBase.isRegistered(message->chat->id)) {
        bot.getApi().sendMessage(message->chat->id, "Вот что я умею", false, 0, mainMenu());
      } else {
        bot.getApi().sendMessage(message->chat->id,
                                 "Зарегистрируйтесь чтобы использовать функции бота. Для регистрации используйте команду /register");
      }
    }
  });

  // Обработка команды homework
  bot.getEvents().onCommand("homework", [&bot, &users, &registrationDataBase](TgBot::Message::Ptr message) {
    if (!users[message->chat->id].isTyping) {
      if (registrationDataBase.isRegistered(message->chat->id)) {
        bot.getApi().sendMessage(message->chat->id, "Доступные команды:", false, 0, homeworkMenu(registrationDataBase.isAdmin(message->chat->id)));
      } else {
        bot.getApi().sendMessage(message->chat->id,
                                 "Зарегистрируйтесь чтобы использовать функции бота. Для регистрации используйте команду /register");
      }
    }
  });

  // Обработка нажатий кнопок
  bot.getEvents().onCallbackQuery([&bot, &registrationDataBase, &users, &homeWorkBases, &currentTime](TgBot::CallbackQuery::Ptr query) {
    if (query->data == "homework") {
      // homework

      bot.getApi().editMessageText("Меню домашки:", query->message->chat->id, query->message->messageId, "", "", false,
                                   homeworkMenu(registrationDataBase.isAdmin(query->message->chat->id)));
    } else if (query->data == "notifications") {
      // notifications
    } else if (query->data == "backToMainMenu") {
      // back to main menu
      bot.getApi().editMessageText("Вот что я умею:", query->message->chat->id, query->message->messageId, "", "", false, mainMenu());
    } else if (query->data == "showHomework") {
      // showHomework
      bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
      std::string group = registrationDataBase.getGroup(query->message->chat->id);
      users[query->message->chat->id].weekPage = HomeworkDataBase::getCurrentWeek(currentTime);
      bot.getApi().sendMessage(query->message->chat->id,
                               std::to_string(users[query->message->chat->id].weekPage) + " неделя\n" +
                                   homeWorkBases[group][users[query->message->chat->id].weekPage - 1].showHomework(),
                               false, 0, showHomeworkMenu());
    } else if (query->data == "addNoteToHomework") {
      // addNoteToHomework

      bot.getApi().editMessageText("Выберете какой тип заметки вы хотите оставить(общие заметки доступны только админам):", query->message->chat->id,
                                   query->message->messageId, "", "", false, noteMenu());

    } else if (query->data == "addHomework") {
      // addHomework
      bot.getApi().deleteMessage(query->message->chat->id, query->message->messageId);
      bot.getApi().sendMessage(query->message->chat->id,
                               "Нашишите мне домашнее задание которое хотите добавить в формате:\nНомер недели:День недели:Предмет:Задание");
      users[query->message->chat->id].process = "homework";
      users[query->message->chat->id].isTyping = true;
    } else if (query->data == "addUserNote") {
      // addUserNote
    } else if (query->data == "addGlobalNote") {
      // addGlobalNote
    } else if (query->data == "prevWeek") {
      // prevWeek
      if (users[query->message->chat->id].weekPage - 1 >= 1) {
        users[query->message->chat->id].weekPage -= 1;
        std::string group = registrationDataBase.getGroup(query->message->chat->id);
        bot.getApi().editMessageText(std::to_string(users[query->message->chat->id].weekPage) + " неделя\n" +
                                         homeWorkBases[group][users[query->message->chat->id].weekPage - 1].showHomework(),
                                     query->message->chat->id, query->message->messageId, "", "", false, showHomeworkMenu());
      }
    } else if (query->data == "nextWeek") {
      // nextWeek
      if (users[query->message->chat->id].weekPage + 1 <= 18) {
        users[query->message->chat->id].weekPage += 1;
        std::string group = registrationDataBase.getGroup(query->message->chat->id);
        bot.getApi().editMessageText(std::to_string(users[query->message->chat->id].weekPage) + " неделя\n" +
                                         homeWorkBases[group][users[query->message->chat->id].weekPage - 1].showHomework(),
                                     query->message->chat->id, query->message->messageId, "", "", false, showHomeworkMenu());
      }
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

  TgBot::BotCommand::Ptr homeworkCommand(new TgBot::BotCommand);
  homeworkCommand->command = "homework";
  homeworkCommand->description = "Домашнее задание";

  TgBot::BotCommand::Ptr notifyCommand(new TgBot::BotCommand);
  notifyCommand->command = "notifications";
  notifyCommand->description = "Уведомления";

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
  button1->text = "Домашнее задание";
  button1->callbackData = "homework";
  row1.push_back(button1);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
  TgBot::InlineKeyboardButton::Ptr button2(new TgBot::InlineKeyboardButton);
  button2->text = "Уведомления";
  button2->callbackData = "notifications";
  row2.push_back(button2);

  keyboard->inlineKeyboard.push_back(row1);
  keyboard->inlineKeyboard.push_back(row2);
  return keyboard;
}

// Функция возвращающая кнопки меню домашнего задания
TgBot::InlineKeyboardMarkup::Ptr homeworkMenu(bool adminAccess) {
  TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row1;
  TgBot::InlineKeyboardButton::Ptr button1(new TgBot::InlineKeyboardButton);
  button1->text = "Посмотреть домашнее задание";
  button1->callbackData = "showHomework";
  row1.push_back(button1);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
  TgBot::InlineKeyboardButton::Ptr button2(new TgBot::InlineKeyboardButton);
  button2->text = "Добавить заметку по домашнему заданию";
  button2->callbackData = "addNoteToHomework";
  row2.push_back(button2);

  keyboard->inlineKeyboard.push_back(row1);
  keyboard->inlineKeyboard.push_back(row2);

  if (adminAccess) {
    std::vector<TgBot::InlineKeyboardButton::Ptr> row3;
    TgBot::InlineKeyboardButton::Ptr button3(new TgBot::InlineKeyboardButton);
    button3->text = "Добавить новое домашнее задание";
    button3->callbackData = "addHomework";
    row3.push_back(button3);

    keyboard->inlineKeyboard.push_back(row3);
  }

  std::vector<TgBot::InlineKeyboardButton::Ptr> row4;
  TgBot::InlineKeyboardButton::Ptr button4(new TgBot::InlineKeyboardButton);
  button4->text = "Меню";
  button4->callbackData = "backToMainMenu";
  row4.push_back(button4);

  keyboard->inlineKeyboard.push_back(row4);

  return keyboard;
}

// Функция возвращающая кнопки меню добавления заметки в дз
TgBot::InlineKeyboardMarkup::Ptr noteMenu() {
  TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row1;
  TgBot::InlineKeyboardButton::Ptr button1(new TgBot::InlineKeyboardButton);
  button1->text = "Добавить личную заметку";
  button1->callbackData = "addUserNote";
  row1.push_back(button1);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
  TgBot::InlineKeyboardButton::Ptr button2(new TgBot::InlineKeyboardButton);
  button2->text = "Добавить общую заметку";
  button2->callbackData = "addGlobalNote";
  row2.push_back(button2);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row3;
  TgBot::InlineKeyboardButton::Ptr button3(new TgBot::InlineKeyboardButton);
  button3->text = "Меню";
  button3->callbackData = "backToMainMenu";
  row3.push_back(button3);

  keyboard->inlineKeyboard.push_back(row1);
  keyboard->inlineKeyboard.push_back(row2);
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
  button1->text = "Предыдущая неделя";
  button1->callbackData = "prevWeek";
  button2->text = "Следующая неделя";
  button2->callbackData = "nextWeek";
  row1.push_back(button1);
  row1.push_back(button2);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
  TgBot::InlineKeyboardButton::Ptr button3(new TgBot::InlineKeyboardButton);
  button3->text = "Меню";
  button3->callbackData = "backToMainMenu";
  row2.push_back(button3);

  keyboard->inlineKeyboard.push_back(row1);
  keyboard->inlineKeyboard.push_back(row2);

  return keyboard;
}

void refreshCurrentTime(boost::posix_time::ptime &currentTime) { currentTime = boost::posix_time::second_clock::universal_time(); }