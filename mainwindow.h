/**
 * @file mainwindow.h
 * @brief Определение класса MainWindow - главного окна приложения
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Table.h"
#include "DatabaseManager.h"
#include <vector>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Главное окно приложения HTML Table Cleaner
 * 
 * Отвечает за UI-поток приложения: загрузку HTML, очистку таблиц,
 * визуализацию результата, сохранение/копирование и экспорт в SQLite.
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
    explicit MainWindow(QWidget *parent = nullptr);

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
     * @brief Запускает очистку и парсинг таблиц из исходного HTML
     * 
     * Формирует обновление текстового результата и визуальной таблицы.
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
     * Очищает исходный/результирующий текст и визуальную таблицу.
     */
    void onClearButtonClicked();

    /**
     * @brief Экспортирует текущие таблицы в файл SQLite
     */
    void onExportToDbClicked();

private:
    Ui::MainWindow *ui;               ///< Объект интерфейса, сгенерированный из .ui
    std::vector<Table> currentTables; ///< Текущий результат: набор очищенных таблиц
    DatabaseManager m_dbManager;

    /**
     * @brief Отрисовывает набор таблиц в resultTableWidget
     */
    void visualizeTables(const std::vector<Table>& tables);

    /**
     * @brief Обновляет resultTextEdit HTML-представлением currentTables
     */
    void updateResultDisplay();

    /**
     * @brief Полностью очищает данные интерфейса и внутреннее состояние
     */
    void clearAllFields();
};

#endif // MAINWINDOW_H
