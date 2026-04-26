/**
 * @file main.cpp
 * @brief Точка входа в приложение HTML Table Cleaner
 * 
 * Инициализирует Qt-приложение и отображает главное окно.
 */

#include "mainwindow.h"

#include <QApplication>

/**
 * @brief Главная функция приложения
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 * @return Код возврата приложения (0 при успешном завершении)
 * 
 * Создает объект QApplication, необходимый для работы любого Qt-приложения,
 * создает и отображает главное окно, затем запускает главный цикл обработки событий.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
