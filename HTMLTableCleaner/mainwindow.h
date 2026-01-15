/**
 * @file mainwindow.h
 * @brief Определение класса MainWindow - главного окна приложения
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Table.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Главное окно приложения HTML Table Cleaner
 * 
 * Наследуется от QMainWindow и управляет пользовательским интерфейсом,
 * обработкой файлов и взаимодействием с пользователем.
 * Использует систему сигналов и слотов Qt для обработки событий.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор главного окна
     * @param parent Родительский виджет (по умолчанию nullptr)
     * 
     * Инициализирует интерфейс, подключает сигналы кнопок к слотам
     * и настраивает горячие клавиши.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Деструктор
     * 
     * Освобождает ресурсы интерфейса.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Обработчик нажатия кнопки "Открыть"
     * 
     * Открывает диалог выбора файла и загружает HTML-код в текстовое поле.
     */
    void onOpenButtonClicked();

    /**
     * @brief Обработчик нажатия кнопки "Очистить HTML"
     * 
     * Обрабатывает исходный HTML-код, извлекает и очищает таблицу,
     * затем отображает результат.
     */
    void onCleanButtonClicked();

    /**
     * @brief Обработчик нажатия кнопки "Сохранить"
     * 
     * Сохраняет очищенный HTML-код в файл.
     */
    void onSaveButtonClicked();

    /**
     * @brief Обработчик нажатия кнопки "Копировать"
     * 
     * Копирует очищенный HTML-код в буфер обмена.
     */
    void onCopyButtonClicked();

    /**
     * @brief Обработчик нажатия кнопки "Очистить поля"
     * 
     * Очищает все текстовые поля после подтверждения пользователя.
     */
    void onClearButtonClicked();

private:
    Ui::MainWindow *ui;       ///< Указатель на объект интерфейса (сгенерирован из .ui файла)
    Table currentTable;       ///< Текущая обработанная таблица

    /**
     * @brief Обновляет отображение результата в текстовом поле
     * 
     * Преобразует объект currentTable в HTML-код и загружает в resultTextEdit.
     */
    void updateResultDisplay();

    /**
     * @brief Очищает все поля и данные
     * 
     * Очищает текстовые поля и объект currentTable, обновляет статусную строку.
     */
    void clearAllFields();
};

#endif // MAINWINDOW_H
