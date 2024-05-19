#include <stdio.h>
#include <tgbot/tgbot.h>

#include "dataBase/dataBase.h"

struct User {
  bool isInRegistration = false;
  bool isInHomework = false;
  bool isInNotifications = false;
};

int main() {
  TgBot::Bot bot("7031000270:AAFA_R8OMb_N3VZteuLNKSpBGfo9b4eVxD4");
  RegistrationDataBase registrationDataBase("registrationDataBase.txt");
  HomeWorkDataBase homeWorkGr25("ИУ5-25Б.txt");
  HomeWorkDataBase homeWorkGr24("ИУ5-24Б.txt");
  std::unordered_map<std::string, HomeWorkDataBase> homeWorkBases{{"ИУ5-25Б", homeWorkGr25}, {"ИУ5-24Б", homeWorkGr24}};
  std::unordered_map<int64_t, User> users;

//   bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
//     TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

//     // Создание кнопок для выбора группы
//     std::vector<TgBot::InlineKeyboardButton::Ptr> row1;
//     TgBot::InlineKeyboardButton::Ptr button1(new TgBot::InlineKeyboardButton);
//     button1->text = "Группа 1";
//     button1->callbackData = "group1";
//     row1.push_back(button1);

//     TgBot::InlineKeyboardButton::Ptr button2(new TgBot::InlineKeyboardButton);
//     button2->text = "Группа 2";
//     button2->callbackData = "group2";
//     row1.push_back(button2);

//     std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
//     TgBot::InlineKeyboardButton::Ptr button3(new TgBot::InlineKeyboardButton);
//     button3->text = "Группа 3";
//     button3->callbackData = "group3";
//     row2.push_back(button3);

//     TgBot::InlineKeyboardButton::Ptr button4(new TgBot::InlineKeyboardButton);
//     button4->text = "Группа 4";
//     button4->callbackData = "group4";
//     row2.push_back(button4);

//     std::vector<TgBot::InlineKeyboardButton::Ptr> row3;
//     TgBot::InlineKeyboardButton::Ptr button5(new TgBot::InlineKeyboardButton);
//     button5->text = "Группа 5";
//     button5->callbackData = "group5";
//     row3.push_back(button5);

//     keyboard->inlineKeyboard.push_back(row1);
//     keyboard->inlineKeyboard.push_back(row2);
//     keyboard->inlineKeyboard.push_back(row3);

//     bot.getApi().sendMessage(message->chat->id, "Выберите вашу группу:", false, 0, keyboard);
//   });

//   // Обработка нажатий на кнопки
//   bot.getEvents().onCallbackQuery([&bot](TgBot::CallbackQuery::Ptr query) {
//     if (query->data == "group1") {
//       bot.getApi().sendMessage(query->message->chat->id, "Вы выбрали группу 1");
//     } else if (query->data == "group2") {
//       bot.getApi().sendMessage(query->message->chat->id, "Вы выбрали группу 2");
//     } else if (query->data == "group3") {
//       bot.getApi().sendMessage(query->message->chat->id, "Вы выбрали группу 3");
//     } else if (query->data == "group4") {
//       bot.getApi().sendMessage(query->message->chat->id, "Вы выбрали группу 4");
//     } else if (query->data == "group5") {
//       bot.getApi().sendMessage(query->message->chat->id, "Вы выбрали группу 5");
//     }
//   });

  bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
  bot.getApi().sendMessage(
    message->chat->id,
  "Привет! \n👋 Я ЭЖС (Электронный Журнал Студента) – ваш помощник в учебе. \n📚 С моей помощью вы можете: \n✅ "
  "Следить за текущими домашними заданиями и дедлайнами \n✅ Отслеживать свою успеваемость \n✅ Получать "
  "уведомления о новых заданиях и оценках \nНачните с команды /register, чтобы пройти регистрацию и начать "
  "использовать все мои возможности!\nУдачи в учебе! 🚀");
  });

  bot.getEvents().onCommand("register", [&bot, &registrationDataBase, &users](TgBot::Message::Ptr message) {
    if (!registrationDataBase.isRegistered(message->chat->id)) {
      users[message->chat->id].isInRegistration = true;
      bot.getApi().sendMessage(message->chat->id, "Введите ваши данные в формате:\nГруппа:ФИО:Пароль");
    } else {
      bot.getApi().sendMessage(message->chat->id, "Вы уже зарегистрированы");
    }
  });

  bot.getEvents().onNonCommandMessage([&bot, &registrationDataBase, &users](TgBot::Message::Ptr message) {
    if (users[message->chat->id].isInRegistration) {
      if (registrationDataBase.registration(message->text, message->chat->id)) {
        bot.getApi().sendMessage(message->chat->id, "Вы успешно зарегистрировались в системе");
      } else {
        bot.getApi().sendMessage(message->chat->id, "Ваше ФИО не было найдено в базе, либо ваш пароль неверен");
      }
      users[message->chat->id].isInRegistration = false;
    }
  });

  bot.getEvents().onCommand("help", [&bot, &registrationDataBase, &users](TgBot::Message::Ptr message) {
    if (!users[message->chat->id].isInRegistration and !users[message->chat->id].isInNotifications and
        !users[message->chat->id].isInHomework) {
      if (registrationDataBase.isRegistered(message->chat->id)) {
        bot.getApi().sendMessage(
            message->chat->id,
            "Вот что я умею:\n📅 /homework – показывает текущие домашние задания и их дедлайны\n🔔 "
            "/notifications – настраивает уведомления о новых заданиях и "
            "оценках\n📝 /add_note – добавляет новую заметку");
      } else {
        bot.getApi().sendMessage(
            message->chat->id,
            "Зарегистрируйтесь чтобы использовать функции бота. Для регистрации используйте команду /register");
      }
    }
  });

  try {
    printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
    TgBot::TgLongPoll longPoll(bot);
    while (true) {
      printf("Long poll started\n");
      longPoll.start();
    }
  } catch (TgBot::TgException& e) {
    printf("error: %s\n", e.what());
  }
  return 0;
}