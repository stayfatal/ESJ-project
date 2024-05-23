#include <stdio.h>
#include <tgbot/tgbot.h>

#include <cstdio>

#include "dataBase/dataBase.h"

struct User {
  std::string process;
  bool isTyping = false;
};

void setCommandsMenu(TgBot::Bot &bot);
TgBot::InlineKeyboardMarkup::Ptr mainMenu();
TgBot::InlineKeyboardMarkup::Ptr homeworkMenu(bool adminAccess);
TgBot::InlineKeyboardMarkup::Ptr noteMenu();

int main() {
  TgBot::Bot bot("7031000270:AAFA_R8OMb_N3VZteuLNKSpBGfo9b4eVxD4");
  RegistrationDataBase registrationDataBase("registrationDataBase.txt");
  HomeWorkDataBase homeWorkGr25("ИУ5-25Б.txt");
  HomeWorkDataBase homeWorkGr24("ИУ5-24Б.txt");
  std::unordered_map<std::string, HomeWorkDataBase> homeWorkBases{{"ИУ5-25Б", homeWorkGr25}, {"ИУ5-24Б", homeWorkGr24}};
  std::unordered_map<int64_t, User> users;
  std::unordered_map<string, bool> isPressed;
  std::unordered_map<int64_t, int32_t> messageIdBuffer;

  setCommandsMenu(bot);  // Установка меню с командами

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
  bot.getEvents().onNonCommandMessage([&bot, &registrationDataBase, &users](TgBot::Message::Ptr message) {
    if (users[message->chat->id].isTyping and users[message->chat->id].process == "registration") {
      if (registrationDataBase.registration(message->text, message->chat->id)) {
        bot.getApi().sendMessage(
            message->chat->id,
            "Вы успешно зарегистрировались в системе, теперь у вас есть возможность пользоваться всеми функциями бота, для этого напишите /menu");
      } else {
        bot.getApi().sendMessage(message->chat->id, "Ваше ФИО не было найдено в базе, либо ваш пароль неверен");
      }
      users[message->chat->id].isTyping = false;
    }
  });

  // Обработка команды menu
  bot.getEvents().onCommand("menu", [&bot, &registrationDataBase, &users, &messageIdBuffer](TgBot::Message::Ptr message) {
    if (!users[message->chat->id].isTyping) {
      if (registrationDataBase.isRegistered(message->chat->id)) {
        messageIdBuffer[message->chat->id] = bot.getApi().sendMessage(message->chat->id, "Вот что я умею", false, 0, mainMenu())->messageId;
      } else {
        bot.getApi().sendMessage(message->chat->id,
                                 "Зарегистрируйтесь чтобы использовать функции бота. Для регистрации используйте команду /register");
      }
    }
  });

  // Обработка команды homework
  bot.getEvents().onCommand("homework", [&bot, &users, &registrationDataBase, &isPressed](TgBot::Message::Ptr message) {
    if (!users[message->chat->id].isTyping) {
      if (registrationDataBase.isRegistered(message->chat->id)) {
        isPressed["showHomework"] = false;
        isPressed["addNoteToHomework"] = false;
        isPressed["addHomework"] = false;

        bot.getApi().sendMessage(message->chat->id, "Доступные команды:", false, 0, homeworkMenu(registrationDataBase.isAdmin(message->chat->id)));
        return;
      } else {
        bot.getApi().sendMessage(message->chat->id,
                                 "Зарегистрируйтесь чтобы использовать функции бота. Для регистрации используйте команду /register");
      }
    }
  });

  // Обработка нажатий кнопок
  bot.getEvents().onCallbackQuery([&bot, &isPressed, &registrationDataBase, &messageIdBuffer](TgBot::CallbackQuery::Ptr query) {
    if (query->data == "homework") {
      isPressed["showHomework"] = false;
      isPressed["addNoteToHomework"] = false;
      isPressed["addHomework"] = false;

      bot.getApi().editMessageText("Меню домашки:", query->message->chat->id, query->message->messageId, "", "", false,
                                   homeworkMenu(registrationDataBase.isAdmin(query->message->chat->id)));
    } else if (query->data == "notifications") {
    } else if (query->data == "backToMainMenu") {
      bot.getApi().editMessageText("Вот что я умею:", query->message->chat->id, query->message->messageId, "", "", false, mainMenu());
    } else if (query->data == "showHomework" and !isPressed["showHomework"]) {
      isPressed["showHomework"] = true;
      bot.getApi().sendMessage(query->message->chat->id, "Вы выбрали группу 1");
    } else if (query->data == "addNoteToHomework" and !isPressed["addNoteToHomework"]) {
      isPressed["addNoteToHomework"] = true;

      bot.getApi().editMessageText("Выберете какой тип заметки вы хотите оставить(общие заметки доступны только админам):", query->message->chat->id,
                        query->message->messageId, "", "", false, noteMenu());

    } else if (query->data == "addHomework" and !isPressed["addHomework"]) {
      isPressed["addHomework"] = true;
      bot.getApi().sendMessage(query->message->chat->id,
                               "Нашишите мне домашнее задание которое хотите добавить в формате:\nНомер недели:День недели:Предмет");
    } else if (query->data == "addUserNote") {
    } else if (query->data == "addGlobalNote") {
    }
  });

  try {
    printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
    TgBot::TgLongPoll longPoll(bot);
    while (true) {
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