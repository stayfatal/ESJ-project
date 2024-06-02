#include "keybords.h"

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
  button2->text = "Настройки уведомлений";
  button2->callbackData = "notifications";
  row2.push_back(button2);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row3;
  TgBot::InlineKeyboardButton::Ptr button3(new TgBot::InlineKeyboardButton);
  button3->text = "Выход из меню";
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
  button1->text = "Посмотреть домашнее задание";
  button1->callbackData = "showHomework";
  row1.push_back(button1);

  keyboard->inlineKeyboard.push_back(row1);

  if (adminAccess) {
    std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
    TgBot::InlineKeyboardButton::Ptr button2(new TgBot::InlineKeyboardButton);
    button2->text = "Добавить новое домашнее задание";
    button2->callbackData = "addHomework";
    row2.push_back(button2);

    keyboard->inlineKeyboard.push_back(row2);
  }

  std::vector<TgBot::InlineKeyboardButton::Ptr> row3;
  TgBot::InlineKeyboardButton::Ptr button3(new TgBot::InlineKeyboardButton);
  button3->text = "Меню";
  button3->callbackData = "backToMainMenu";
  row3.push_back(button3);

  keyboard->inlineKeyboard.push_back(row3);

  return keyboard;
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

TgBot::InlineKeyboardMarkup::Ptr notificationsMenu(bool isOn1, bool isOn2, bool adminAccess) {
  TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row1;
  TgBot::InlineKeyboardButton::Ptr button1(new TgBot::InlineKeyboardButton);
  std::string isOnStr1 = (isOn1 ? "ON" : "OFF");
  button1->text = "Уведомление о сдаче домашнего задания -> " + isOnStr1;
  button1->callbackData = "advanceNotice";
  row1.push_back(button1);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row2;
  TgBot::InlineKeyboardButton::Ptr button2(new TgBot::InlineKeyboardButton);
  std::string isOnStr2 = (isOn2 ? "ON" : "OFF");
  button2->text = "Уведомления от администраторов -> " + isOnStr2;
  button2->callbackData = "adminNotice";
  row2.push_back(button2);

  if (adminAccess) {
    std::vector<TgBot::InlineKeyboardButton::Ptr> row3;
    TgBot::InlineKeyboardButton::Ptr button3(new TgBot::InlineKeyboardButton);
    button3->text = "Отправить сообщение в свою группу";
    button3->callbackData = "globalMessage";
    row3.push_back(button3);

    keyboard->inlineKeyboard.push_back(row3);
  }

  std::vector<TgBot::InlineKeyboardButton::Ptr> row4;
  TgBot::InlineKeyboardButton::Ptr button4(new TgBot::InlineKeyboardButton);
  button4->text = "Меню";
  button4->callbackData = "backToMainMenu";
  row4.push_back(button4);

  keyboard->inlineKeyboard.push_back(row1);
  keyboard->inlineKeyboard.push_back(row2);
  keyboard->inlineKeyboard.push_back(row4);

  return keyboard;
}

TgBot::InlineKeyboardMarkup::Ptr returnToMenu() {
  TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);

  std::vector<TgBot::InlineKeyboardButton::Ptr> row1;
  TgBot::InlineKeyboardButton::Ptr button1(new TgBot::InlineKeyboardButton);
  button1->text = "Обратно в меню";
  button1->callbackData = "backToMainMenu";
  row1.push_back(button1);

  keyboard->inlineKeyboard.push_back(row1);

  return keyboard;
}
