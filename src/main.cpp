#include <stdio.h>
#include <tgbot/tgbot.h>
#include "dataBase/dataBase.h"
#include <iostream>
#include <fstream>

int main() {
    TgBot::Bot bot("7031000270:AAFA_R8OMb_N3VZteuLNKSpBGfo9b4eVxD4");
    RegistrationDataBase users("users.txt");
    HomeWorkDataBase homeWork("homeWork.txt");
    std::unordered_map<int64_t, bool> registrationState;

    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Привет! \n👋 Я ЭЖС (Электронный Журнал Студента) – ваш помощник в учебе. \n📚 С моей помощью вы можете: \n✅ Следить за текущими домашними заданиями и дедлайнами \n✅ Отслеживать свою успеваемость \n✅ Получать уведомления о новых заданиях и оценках \nНачните с команды /register, чтобы пройти регистрацию и начать использовать все мои возможности!\nУдачи в учебе! 🚀");
    });

    bot.getEvents().onCommand("register", [&bot, &users, &registrationState](TgBot::Message::Ptr message) {
        if(!users.isRegistered(message->chat->id)){
            registrationState[message->chat->id] = true;
            bot.getApi().sendMessage(message->chat->id, "Введите ваши данные в формате:\nГруппа:ФИО:Пароль");
        } else {
            bot.getApi().sendMessage(message->chat->id, "Вы уже зарегистрированы");
        }
    });

    bot.getEvents().onNonCommandMessage([&bot, &users, &registrationState](TgBot::Message::Ptr message) {
        if (registrationState[message->chat->id]) {
            if (users.registration(message->text, message->chat->id)) {
                bot.getApi().sendMessage(message->chat->id, "Вы успешно зарегистрировались в системе");
            } else {
                bot.getApi().sendMessage(message->chat->id, "Ваше ФИО не было найдено в базе, либо ваш пароль неверен");
            }
            registrationState[message->chat->id] = false;
        }
    });

    bot.getEvents().onCommand("help", [&bot,&users,&registrationState](TgBot::Message::Ptr message) {
        if(users.isRegistered(message->chat->id)){
            registrationState[message->chat->id] = true;
            bot.getApi().sendMessage(message->chat->id, "Вот что я умею:\n📅 /homework – показывает текущие домашние задания и их дедлайны\n📈 /grades – показывает ваши текущие оценки и успеваемость\n🔔 /notifications – настраивает уведомления о новых заданиях и оценках\n📝 /add_note – добавляет новую заметку");
        } else {
            bot.getApi().sendMessage(message->chat->id, "Зарегистрируйтесь чтобы использовать функции бота. Для регистрации используйте команду /register");
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