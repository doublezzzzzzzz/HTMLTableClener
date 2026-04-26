/**
 * @file Table.cpp
 * @brief Реализация модели таблицы для очищенных HTML-данных
 */

#include "Table.h"
#include <sstream>
#include <algorithm>

void Table::addRow(const std::vector<CellPtr>& row) {
    rows.push_back(row);
}

CellPtr Table::getCell(size_t row, size_t col) const {
    if (row >= rows.size() || col >= rows[row].size()) {
        return nullptr;
    }
    return rows[row][col];
}

std::string Table::getCellContent(size_t row, size_t col) const {
    auto cell = getCell(row, col);
    return cell ? cell->getContent() : "";
}

void Table::clear() {
    rows.clear();
}

size_t Table::getMaxColumnCount() const {
    size_t max = 0;
    for (const auto& row : rows) {
        max = std::max(max, row.size());
    }
    return max;
}

std::string Table::toHtml() const {
    std::ostringstream html;

    html << "<table>\n";

    for (size_t r = 0; r < rows.size(); ++r) {
        html << "  <tr>\n";

        for (size_t c = 0; c < rows[r].size(); ++c) {
            const auto& cell = rows[r][c];
            if (!cell) continue;

            // Пропускаем ReferenceCell: область уже покрыта главной ячейкой.
            auto target = cell->getTarget();
            if (target && target != cell) {
                continue;
            }

            html << "    <" << (cell->getIsHeader() ? "th" : "td");

            // Добавляем атрибуты только для span > 1.
            if (cell->getRowspan() > 1)
                html << " rowspan=\"" << cell->getRowspan() << "\"";
            if (cell->getColspan() > 1)
                html << " colspan=\"" << cell->getColspan() << "\"";

            html << ">" << cell->getContent() << "</"
                 << (cell->getIsHeader() ? "th" : "td") << ">\n";
        }
        html << "  </tr>\n";
    }

    html << "</table>";
    return html.str();
}
