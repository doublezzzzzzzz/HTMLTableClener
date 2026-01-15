/**
 * @file Table.h
 * @brief Определение класса Table - структуры HTML-таблицы
 */

#ifndef TABLE_H
#define TABLE_H

#include "Cell.h"
#include <vector>

/**
 * @class Table
 * @brief Представляет структуру HTML-таблицы
 * 
 * Класс хранит таблицу как контейнер строк, где каждая строка
 * представляет собой вектор ячеек. Предоставляет методы для
 * добавления строк, очистки и преобразования в HTML-код.
 */
class Table {
public:
    /**
     * @brief Конструктор по умолчанию
     * 
     * Создает пустую таблицу.
     */
    Table() = default;

    /**
     * @brief Добавляет строку в таблицу
     * @param row Вектор ячеек, представляющий строку таблицы
     */
    void addRow(const std::vector<Cell>& row);

    /**
     * @brief Возвращает все строки таблицы
     * @return Константная ссылка на контейнер строк
     * 
     * Позволяет получить доступ к структуре таблицы только для чтения.
     */
    const std::vector<std::vector<Cell>>& getRows() const { return rows; }

    /**
     * @brief Очищает таблицу, удаляя все строки
     */
    void clear() { rows.clear(); }

    /**
     * @brief Преобразует таблицу в HTML-код
     * @return Строка с валидным HTML-кодом таблицы
     * 
     * Генерирует HTML-код с тегами <table>, <tr>, <td>, <th>.
     * Сохраняет атрибуты rowspan и colspan, если они больше 1.
     */
    std::string toHtml() const;

private:
    std::vector<std::vector<Cell>> rows;  ///< Контейнер строк таблицы (каждая строка - вектор ячеек)
};

#endif // TABLE_H
