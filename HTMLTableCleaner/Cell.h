/**
 * @file Cell.h
 * @brief Определение структуры Cell - ячейки HTML-таблицы
 */

#ifndef CELL_H
#define CELL_H

#include <string>

/**
 * @struct Cell
 * @brief Представляет ячейку HTML-таблицы
 * 
 * Структура хранит содержимое ячейки, её атрибуты (rowspan, colspan)
 * и информацию о том, является ли ячейка заголовком.
 */
struct Cell {
    int rowspan = 1;              ///< Количество строк, которые занимает ячейка (по умолчанию 1)
    int colspan = 1;              ///< Количество столбцов, которые занимает ячейка (по умолчанию 1)
    std::string content;          ///< Текстовое содержимое ячейки
    bool isHeader = false;        ///< Флаг заголовка (true = <th>, false = <td>)

    /**
     * @brief Конструктор по умолчанию
     * 
     * Инициализирует все поля значениями по умолчанию.
     */
    Cell() = default;

    /**
     * @brief Конструктор с параметрами
     * @param content Текстовое содержимое ячейки
     * @param rowspan Количество строк, которые занимает ячейка (по умолчанию 1)
     * @param colspan Количество столбцов, которые занимает ячейка (по умолчанию 1)
     * @param isHeader Флаг заголовка (по умолчанию false)
     */
    Cell(const std::string& content, int rowspan = 1, int colspan = 1, bool isHeader = false)
        : content(content), rowspan(rowspan), colspan(colspan), isHeader(isHeader) {}
};

#endif // CELL_H

