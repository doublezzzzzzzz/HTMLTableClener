/**
 * @file Table.cpp
 * @brief Реализация методов класса Table
 */

#include "Table.h"
#include <sstream>

/**
 * @brief Добавляет строку ячеек в таблицу
 * @param row Вектор ячеек, представляющий строку таблицы
 * 
 * Добавляет новую строку в конец таблицы.
 */
void Table::addRow(const std::vector<Cell>& row) {
    rows.push_back(row);
}

/**
 * @brief Преобразует таблицу в HTML-код
 * @return Строка с валидным HTML-кодом таблицы
 * 
 * Генерирует HTML-код следующего формата:
 * - Открывающий тег: <table border="1">
 * - Для каждой строки: <tr>...</tr>
 * - Для каждой ячейки: <td>...</td> или <th>...</th>
 * - Атрибуты rowspan и colspan добавляются только если > 1
 * - Закрывающий тег: </table>
 * 
 * Использует отступы для читаемости кода (2 пробела для строк, 4 для ячеек).
 */
std::string Table::toHtml() const {
    std::ostringstream html;
    html << "<table border=\"1\">\n";

    // Обрабатываем каждую строку таблицы
    for (const auto& row : rows) {
        html << "  <tr>\n";
        
        // Обрабатываем каждую ячейку в строке
        for (const auto& cell : row) {
            html << "    <";
            // Используем <th> для заголовков, <td> для обычных ячеек
            html << (cell.isHeader ? "th" : "td");

            // Добавляем атрибуты rowspan и colspan, если они больше 1
            if (cell.rowspan > 1)
                html << " rowspan=\"" << cell.rowspan << "\"";
            if (cell.colspan > 1)
                html << " colspan=\"" << cell.colspan << "\"";

            html << ">" << cell.content << "</";
            html << (cell.isHeader ? "th" : "td") << ">\n";
        }
        html << "  </tr>\n";
    }

    html << "</table>";
    return html.str();
}
