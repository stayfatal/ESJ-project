#include "commands.h"

void handleStartCommand(TgBot::Bot &bot, std::unordered_map<int64_t, User> &users, TgBot::Message::Ptr message) {
  if (!users[message->chat->id].isTyping) {
    bot.getApi().sendMessage(message->chat->id,
                             "Привет! \n👋 Я ЭЖС (Электронный Журнал Студента) – ваш помощник в учебе. \n📚 С моей помощью вы можете: \n✅ "
                             "Следить за текущими домашними заданиями и дедлайнами \n✅ Отслеживать свою успеваемость \n✅ Получать "
                             "уведомления о новых заданиях и оценках \nНачните с команды /register, чтобы пройти регистрацию и начать "
                             "использовать все мои возможности!\nУдачи в учебе! 🚀");
  }
}

void handleRegisterCommand(TgBot::Bot &bot, UserDataBase &userDataBase, std::unordered_map<int64_t, User> &users, TgBot::Message::Ptr message) {
  if (!userDataBase.isRegistered(message->chat->id)) {
    users[message->chat->id].isTyping = true;
    users[message->chat->id].process = Process::ProcessEnum::registration;
    bot.getApi().sendMessage(message->chat->id,
                             "Введите ваши данные в формате:\nГруппа:ФИО:Пароль\nex: ИУ7-15Б:Иванов Иван Иванович:123\n/cancel для отмены");
  } else {
    bot.getApi().sendMessage(message->chat->id, "Вы уже зарегистрированы");
  }
}

void handleMenuCommand(TgBot::Bot &bot, UserDataBase &userDataBase, std::unordered_map<int64_t, User> &users, TgBot::Message::Ptr message) {
  if (!users[message->chat->id].isTyping) {
    if (userDataBase.isRegistered(message->chat->id)) {
      bot.getApi().sendMessage(message->chat->id, "Вот что я умею", false, 0, mainMenu());
    } else {
      bot.getApi().sendMessage(message->chat->id, "Зарегистрируйтесь чтобы использовать функции бота. Для регистрации используйте команду /register");
    }
  }
}

void handleCancelCommand(TgBot::Bot &bot, UserDataBase &userDataBase, std::unordered_map<int64_t, User> &users, TgBot::Message::Ptr message) {
  if (users[message->chat->id].isTyping) {
    users[message->chat->id].isTyping = false;
    bot.getApi().sendMessage(message->chat->id, "Операция отменена", false, 0, returnToMenu());
  }
}