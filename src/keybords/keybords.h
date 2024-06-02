#include <tgbot/tgbot.h>

TgBot::InlineKeyboardMarkup::Ptr mainMenu();

TgBot::InlineKeyboardMarkup::Ptr homeworkMenu(bool adminAccess);

TgBot::InlineKeyboardMarkup::Ptr showHomeworkMenu();

TgBot::InlineKeyboardMarkup::Ptr notificationsMenu(bool isOn1, bool isOn2, bool adminAccess);

TgBot::InlineKeyboardMarkup::Ptr returnToMenu();