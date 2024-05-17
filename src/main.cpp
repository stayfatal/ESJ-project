#include <stdio.h>
#include <tgbot/tgbot.h>

int main() {
    TgBot::Bot bot("7031000270:AAFA_R8OMb_N3VZteuLNKSpBGfo9b4eVxD4");
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Привет! \n👋 Я ЭЖС (Электронный Журнал Студента) – ваш помощник в учебе. \n📚 С моей помощью вы можете: \n✅ Следить за текущими домашними заданиями и дедлайнами \n✅ Отслеживать свою успеваемость \n✅ Получать уведомления о новых заданиях и оценках \nНачните с команды /help, чтобы узнать, как я могу вам помочь! Удачи в учебе! 🚀");
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