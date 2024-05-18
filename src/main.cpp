#include <stdio.h>
#include <tgbot/tgbot.h>
#include "dataBase/dataBase.h"
#include <iostream>
#include <fstream>

void editDB(std::ifstream in,int line,std::string newLine);

int main() {
    TgBot::Bot bot("7031000270:AAFA_R8OMb_N3VZteuLNKSpBGfo9b4eVxD4");
    DataBase users("users.txt");
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Привет! \n👋 Я ЭЖС (Электронный Журнал Студента) – ваш помощник в учебе. \n📚 С моей помощью вы можете: \n✅ Следить за текущими домашними заданиями и дедлайнами \n✅ Отслеживать свою успеваемость \n✅ Получать уведомления о новых заданиях и оценках \nНачните с команды /register, чтобы пройти регистрацию и начать использовать все мои возможности!\nУдачи в учебе! 🚀");
    });
    bot.getEvents().onCommand("register", [&bot,&users](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Введите ваши данные в формате:\nГруппа:ФИО:Пароль");
        bot.getEvents().onNonCommandMessage([&bot,&users](TgBot::Message::Ptr message){
            if(users.findLine(message->text)!=-1){
                users.editDb(users.findLine(message->text)-1,std::to_string(message->chat->id),false);
                bot.getApi().sendMessage(message->chat->id, "Вы успешно зарегистрировались в системе");
            }else{
                bot.getApi().sendMessage(message->chat->id, "Ваше ФИО не было найдено в базе, либо ваш пароль неверен");
            }
        });
    });
    bot.getEvents().onCommand("help", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Вот что я умею:\n📅 /homework – показывает текущие домашние задания и их дедлайны\n📈 /grades – показывает ваши текущие оценки и успеваемость\n🔔 /notifications – настраивает уведомления о новых заданиях и оценках\n📝 /add_note – добавляет новую заметку");
    });
    // bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
    //     printf("User wrote %s\n", message->text.c_str());
    //     if (StringTools::startsWith(message->text, "/start")) {
    //         return;
    //     }
    //     bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    // });
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

void editDB(std::ifstream in,int line,std::string newLine){

}