/**
 * @file Table.h
 * @brief Структура таблицы с использованием полиморфных ячеек
 */

#ifndef TABLE_H
#define TABLE_H

#include "Cell.h"
#include <vector>
#include <memory>

class Table {
public:
    Table() = default;

    /// Добавляет строку виртуальной сетки таблицы.
    void addRow(const std::vector<CellPtr>& row);

    /// Возвращает все строки таблицы (только чтение).
    const std::vector<std::vector<CellPtr>>& getRows() const { return rows; }

    /// Безопасный доступ к ячейке по индексам.
    CellPtr getCell(size_t row, size_t col) const;

    /// Возвращает содержимое ячейки (или пустую строку).
    std::string getCellContent(size_t row, size_t col) const;

    /// Очищает таблицу.
    void clear();

    /// Сериализует таблицу в HTML.
    std::string toHtml() const;

    /// Количество строк виртуальной сетки.
    size_t getRowCount() const { return rows.size(); }

    /// Максимальное количество колонок среди строк.
    size_t getMaxColumnCount() const;

private:
    /// Виртуальная сетка таблицы с поддержкой merged ячеек.
    std::vector<std::vector<CellPtr>> rows;
};

#endif // TABLE_H
