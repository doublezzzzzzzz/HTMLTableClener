/**
 * @file TableParser.cpp
 * @brief Реализация очистки и парсинга HTML-таблиц
 */

#include "TableParser.h"
#include "Cell.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <memory>

using namespace std;

// ============================================================================
// Вспомогательные функции
// ============================================================================

/**
 * @brief Находит соответствующий закрывающий тег с учётом вложенности
 */
size_t findMatchingCloseTag(const string& html, const string& tag, size_t start_pos) {
    size_t pos = start_pos;
    string open_tag = "<" + tag;
    string close_tag = "</" + tag + ">";
    int depth = 0;

    while (pos < html.length()) {
        size_t open_pos = html.find(open_tag, pos);
        size_t close_pos = html.find(close_tag, pos);

        if (open_pos == string::npos) open_pos = html.length();
        if (close_pos == string::npos) close_pos = html.length();

        if (open_pos < close_pos) {
            if (open_pos + open_tag.length() < html.length()) {
                char next_char = html[open_pos + open_tag.length()];
                if (next_char == ' ' || next_char == '>' || next_char == '\t' ||
                    next_char == '\n' || next_char == '\r' || next_char == '/') {
                    depth++;
                }
            }
            pos = open_pos + 1;
        } else if (close_pos < open_pos) {
            if (depth == 0) {
                return close_pos;
            }
            depth--;
            pos = close_pos + 1;
        } else {
            break;
        }
    }
    return string::npos;
}

/**
 * @brief Извлекает значение атрибута из HTML-тега
 */
string extractAttribute(const string& html, const string& attr_name, size_t pos) {
    size_t attr_start = html.find(attr_name, pos);
    if (attr_start == string::npos) return "1";

    size_t equals_pos = html.find('=', attr_start);
    if (equals_pos == string::npos) return "1";

    size_t value_start = equals_pos + 1;
    while (value_start < html.length() &&
           (html[value_start] == ' ' || html[value_start] == '\t' ||
            html[value_start] == '"' || html[value_start] == '\'')) {
        value_start++;
    }
    if (value_start >= html.length()) return "1";

    size_t value_end = value_start;
    while (value_end < html.length() &&
           html[value_end] != ' ' && html[value_end] != '\t' &&
           html[value_end] != '>' && html[value_end] != '"' && html[value_end] != '\'') {
        value_end++;
    }
    return html.substr(value_start, value_end - value_start);
}

/**
 * @brief Рекурсивно удаляет теги форматирования, сохраняя текст
 */
string removeFormattingTags(const string& text) {
    string result = text;
    vector<pair<string, string>> formatting_tags = {
        {"<b>", "</b>"}, {"<B>", "</B>"}, {"<strong>", "</strong>"}, {"<STRONG>", "</STRONG>"},
        {"<i>", "</i>"}, {"<I>", "</I>"}, {"<em>", "</em>"}, {"<EM>", "</EM>"},
        {"<u>", "</u>"}, {"<U>", "</U>"}, {"<ins>", "</ins>"}, {"<INS>", "</INS>"},
        {"<s>", "</s>"}, {"<S>", "</S>"}, {"<strike>", "</strike>"}, {"<STRIKE>", "</STRIKE>"},
        {"<del>", "</del>"}, {"<DEL>", "</DEL>"},
        {"<span", "</span>"}, {"<SPAN", "</SPAN>"},
        {"<font", "</font>"}, {"<FONT", "</FONT>"}
    };

    for (const auto& tag_pair : formatting_tags) {
        size_t pos = 0;
        while ((pos = result.find(tag_pair.first, pos)) != string::npos) {
            size_t tag_end = result.find('>', pos);
            if (tag_end == string::npos) break;

            size_t close_tag_start = result.find(tag_pair.second, tag_end);
            if (close_tag_start == string::npos) {
                result.erase(pos, tag_end - pos + 1);
            } else {
                string inner = result.substr(tag_end + 1, close_tag_start - tag_end - 1);
                inner = removeFormattingTags(inner);
                result.replace(pos, close_tag_start - pos + tag_pair.second.length(), inner);
                pos += inner.length();
            }
        }
    }
    return result;
}

// ============================================================================
// Методы TableParser
// ============================================================================

/**
 * @brief Комплексная очистка содержимого ячейки
 */
string TableParser::cleanCellContent(const string& content) {
    string result = content;

    // 1. Удаляем Word-специфичные теги <o:p>...</o:p>
    size_t pos = 0;
    while ((pos = result.find("<o:p>", pos)) != string::npos) {
        size_t end = result.find("</o:p>", pos);
        if (end != string::npos) {
            result.erase(pos, end - pos + 6);
        } else {
            pos += 5;
        }
    }

    // 2. Удаляем все атрибуты из <p> и </p> тегов
    pos = 0;
    while ((pos = result.find("<p", pos)) != string::npos) {
        size_t tag_end = result.find('>', pos);
        if (tag_end == string::npos) break;

        if (pos + 1 < result.length() && result[pos+1] == '/') {
            // Закрывающий тег: </p ...> → </p>
            result.replace(pos, tag_end - pos + 1, "</p>");
            pos = pos + 4;
        } else {
            // Открывающий тег: <p ...> → <p>
            result.replace(pos, tag_end - pos + 1, "<p>");
            pos = pos + 3;
        }
    }

    // 3. Удаляем остальные теги форматирования
    result = removeFormattingTags(result);

    // 4. Заменяем HTML-сущности
    vector<pair<string, string>> entities = {
        {"&nbsp;", " "}, {"&amp;", "&"}, {"&lt;", "<"}, {"&gt;", ">"},
        {"&quot;", "\""}, {"&apos;", "'"}, {"&#39;", "'"}, {"&#34;", "\""},
        {"&#160;", " "}, {"&ensp;", " "}, {"&emsp;", " "}
    };
    for (const auto& e : entities) {
        size_t p = 0;
        while ((p = result.find(e.first, p)) != string::npos) {
            result.replace(p, e.first.length(), e.second);
            p += e.second.length();
        }
    }

    // 5. Удаляем пустые параграфы
    pos = 0;
    while ((pos = result.find("<p></p>", pos)) != string::npos) {
        result.erase(pos, 7);
    }
    pos = 0;
    while ((pos = result.find("<p> </p>", pos)) != string::npos) {
        result.erase(pos, 8);
    }

    // 6. Нормализация пробелов
    string cleaned;
    bool last_space = false;
    for (char c : result) {
        if (isspace(static_cast<unsigned char>(c))) {
            if (!last_space && !cleaned.empty()) {
                cleaned += ' ';
                last_space = true;
            }
        } else {
            cleaned += c;
            last_space = false;
        }
    }

    // Trim (обрезка пробелов по краям)
    while (!cleaned.empty() && isspace(static_cast<unsigned char>(cleaned.front())))
        cleaned.erase(0, 1);
    while (!cleaned.empty() && isspace(static_cast<unsigned char>(cleaned.back())))
        cleaned.pop_back();

    // 7. Финальная проверка: закрываем <p>, если он открыт, но не закрыт
    if (!cleaned.empty() && cleaned.find("<p>") == 0 && cleaned.rfind("</p>") == string::npos) {
        cleaned += "</p>";
    }

    // Если после всего остался только пустой <p></p> — возвращаем пустую строку
    if (cleaned == "<p></p>") {
        cleaned = "";
    }

    return cleaned;
}

/**
 * @brief Удаляет Word-специфичные стили
 */
string TableParser::removeWordStyles(const string& html) {
    string result = html;

    // Удаляем class="..."
    size_t pos = 0;
    while ((pos = result.find("class=\"", pos)) != string::npos) {
        size_t end = result.find('"', pos + 7);
        if (end == string::npos) break;
        result.erase(pos, end - pos + 1);
    }

    // Удаляем style="..."
    pos = 0;
    while ((pos = result.find("style=\"", pos)) != string::npos) {
        size_t end = result.find('"', pos + 7);
        if (end != string::npos) {
            result.erase(pos, end - pos + 1);
        } else {
            pos += 7;
        }
    }

    // Удаляем namespace декларации
    vector<string> ns = {"xmlns:v=", "xmlns:o=", "xmlns:w=", "xmlns:m=", "v:lang=", "o:lang=", "lang=", "xmlns="};
    for (const auto& n : ns) {
        pos = 0;
        while ((pos = result.find(n, pos)) != string::npos) {
            size_t qs = result.find('"', pos);
            if (qs == string::npos) break;
            size_t qe = result.find('"', qs + 1);
            if (qe == string::npos) break;
            result.erase(pos, qe - pos + 1);
        }
    }
    return result;
}

/**
 * @brief Нормализация HTML: удаление комментариев, CDATA, лишних тегов
 */
string TableParser::normalizeHtml(const string& html) {
    string result = html;

    size_t pos = 0;
    while ((pos = result.find("<![if", pos)) != string::npos) {
        size_t end = result.find("<![endif]-->", pos);
        if (end == string::npos) {
            end = result.find("><![endif]", pos);
            if (end == string::npos) {
                end = result.find("<![endif]", pos);
            }
        }
        if (end != string::npos) {
            end += 11; // длина "<![endif]>"
            result.erase(pos, end - pos);
        } else {
            pos += 5;
        }
    }

    // Также удаляем отдельные <![endif]> если остались
    while ((pos = result.find("<![endif]>", pos)) != string::npos) {
        result.erase(pos, 10);
    }

    // Удаляем комментарии
    while ((pos = result.find("<!--", pos)) != string::npos) {
        size_t end = result.find("-->", pos);
        if (end == string::npos) break;
        result.erase(pos, end - pos + 3);
    }

    // Удаляем CDATA
    pos = 0;
    while ((pos = result.find("<![CDATA[", pos)) != string::npos) {
        size_t end = result.find("]]>", pos);
        if (end == string::npos) break;
        result.erase(pos, end - pos + 3);
    }

    // Оставляем только разрешённые теги
    string allowed[] = {"<table", "</table>", "<tr", "</tr>", "<td", "</td>", "<th", "</th>", "<p", "</p>", "<br", "<tbody", "</tbody>"};

    string temp;
    pos = 0;
    while (pos < result.length()) {
        if (result[pos] == '<') {
            size_t end = result.find('>', pos);
            if (end == string::npos) {
                temp += result.substr(pos);
                break;
            }
            string tag = result.substr(pos, end - pos + 1);
            string tag_low = tag;
            transform(tag_low.begin(), tag_low.end(), tag_low.begin(), ::tolower);

            bool keep = false;
            for (const auto& a : allowed) {
                if (tag_low.find(a) == 0) {
                    keep = true;
                    break;
                }
            }
            if (keep) temp += tag;
            pos = end + 1;
        } else {
            temp += result[pos++];
        }
    }
    result = temp;

    // Удаляем обёртки <tbody>
    while ((pos = result.find("<tbody>", pos)) != string::npos) {
        size_t end = result.find("</tbody>", pos);
        if (end == string::npos) break;
        string inner = result.substr(pos + 7, end - pos - 7);
        result.replace(pos, end - pos + 8, inner);
    }
    return result;
}

/**
 * @brief Извлекает таблицу из Word HTML
 */
string TableParser::extractTableFromWordHtml(const string& html) {
    string normalized = normalizeHtml(html);
    size_t start = normalized.find("<table");
    if (start == string::npos) return "";

    size_t end = findMatchingCloseTag(normalized, "table", start);
    if (end == string::npos) {
        end = normalized.find("</table>", start);
        if (end == string::npos) return "";
        end += 8;
    } else {
        end += 8;
    }
    string table = normalized.substr(start, end - start);
    return removeWordStyles(table);
}

/**
 * @brief Извлекает значение rowspan
 */
int TableParser::extractRowspan(const string& html, size_t pos) {
    string val = extractAttribute(html, "rowspan", pos);
    try { return val == "1" ? 1 : stoi(val); }
    catch (...) { return 1; }
}

/**
 * @brief Извлекает значение colspan
 */
int TableParser::extractColspan(const string& html, size_t pos) {
    string val = extractAttribute(html, "colspan", pos);
    try { return val == "1" ? 1 : stoi(val); }
    catch (...) { return 1; }
}

// ============================================================================
// Ключевая функция: заполнение сетки с учетом merged ячеек.
// ============================================================================

/**
 * @brief Заполняет виртуальную сетку с учетом rowspan/colspan
 */
void fillGridWithMergedCells(vector<vector<CellPtr>>& grid,
                             size_t startRow, size_t startCol,
                             const CellPtr& realCell,
                             int rowspan, int colspan) {
    for (int r = 0; r < rowspan; ++r) {
        for (int c = 0; c < colspan; ++c) {
            size_t gr = startRow + r;
            size_t gc = startCol + c;

            // Динамически расширяем сетку.
            while (grid.size() <= gr) grid.emplace_back();
            while (grid[gr].size() <= gc) grid[gr].push_back(nullptr);

            if (r == 0 && c == 0) {
                grid[gr][gc] = realCell;  // Главная ячейка.
            } else {
                grid[gr][gc] = make_shared<ReferenceCell>(realCell);  // Ячейка-ссылка.
            }
        }
    }
}

/**
 * @brief Парсит HTML одной таблицы в виртуальную сетку
 */
Table TableParser::parseSimpleHtml(const string& html) {
    Table table;
    vector<vector<CellPtr>> grid;  // Виртуальная сетка: [row][col] -> CellPtr.

    if (html.empty()) return table;

    size_t pos = 0;
    size_t rowIdx = 0;

    while ((pos = html.find("<tr", pos)) != string::npos) {
        size_t tr_end = findMatchingCloseTag(html, "tr", pos);
        if (tr_end == string::npos) {
            tr_end = html.find("</tr>", pos);
            if (tr_end == string::npos) break;
            tr_end += 5;
        } else {
            tr_end += 5;
        }

        string row_html = html.substr(pos, tr_end - pos);
        size_t cell_pos = 0;
        size_t colIdx = 0;

        while (true) {
            size_t td_pos = row_html.find("<td", cell_pos);
            size_t th_pos = row_html.find("<th", cell_pos);

            if (td_pos == string::npos && th_pos == string::npos) break;

            bool isHeader = (th_pos != string::npos && (td_pos == string::npos || th_pos < td_pos));
            size_t cstart = isHeader ? th_pos : td_pos;
            string tag = isHeader ? "th" : "td";

            size_t cend = findMatchingCloseTag(row_html, tag, cstart);
            if (cend == string::npos) {
                cend = row_html.find("</" + tag + ">", cstart);
                if (cend == string::npos) break;
                cend += tag.length() + 3;
            } else {
                cend += tag.length() + 3;
            }

            string cell_html = row_html.substr(cstart, cend - cstart);
            int rowspan = extractRowspan(cell_html, 0);
            int colspan = extractColspan(cell_html, 0);

            // Извлекаем контент
            size_t cs = cell_html.find('>');
            string content = "";
            if (cs != string::npos) {
                size_t ce = cell_html.rfind('<');
                if (ce > cs) {
                    content = cleanCellContent(cell_html.substr(cs + 1, ce - cs - 1));
                }
            }

            // Фильтрация фиктивных Word-ячеек.
            string cell_html_lower = cell_html;
            transform(cell_html_lower.begin(), cell_html_lower.end(), cell_html_lower.begin(), ::tolower);

            bool isFakeCell = false;

            // Проверка Word-атрибутов технических ячеек.
            if (cell_html_lower.find("width=0") != string::npos &&
                cell_html_lower.find("border:none") != string::npos) {
                isFakeCell = true;
            }

            // Дополнительная проверка для пустых ячеек.
            if (content.empty() || content == " " || content == "<p></p>") {
                if (cell_html_lower.find("supportmisalignedrows") != string::npos ||
                    cell_html_lower.find("v:ext=") != string::npos) {
                    isFakeCell = true;
                }
            }

            if (isFakeCell) {
                // Не добавляем фиктивную ячейку в сетку.
                cell_pos = cend;
                continue;
            }

            // Создаем RealCell для исходной ячейки.
            auto realCell = make_shared<RealCell>(content, rowspan, colspan, isHeader);

            // Находим первую свободную колонку в текущей строке.
            while (grid.size() <= rowIdx) grid.emplace_back();
            while (colIdx < grid[rowIdx].size() && grid[rowIdx][colIdx] != nullptr) {
                ++colIdx;
            }

            // Заполняем сетку с учетом rowspan/colspan.
            fillGridWithMergedCells(grid, rowIdx, colIdx, realCell, rowspan, colspan);

            colIdx += colspan;
            cell_pos = cend;
        }
        ++rowIdx;
        pos = tr_end;
    }

    // Переносим готовую сетку в Table.
    for (auto& row : grid) {
        table.addRow(row);
    }

    return table;
}

/**
 * @brief Парсит все таблицы из HTML
 */
std::vector<Table> TableParser::parseMultipleTables(const string& html) {
    std::vector<Table> tables;

    if (html.empty()) return tables;

    string normalized = normalizeHtml(html);
    size_t pos = 0;

    // Ищем все теги <table>.
    while ((pos = normalized.find("<table", pos)) != string::npos) {
        size_t end = findMatchingCloseTag(normalized, "table", pos);
        if (end == string::npos) {
            end = normalized.find("</table>", pos);
            if (end == string::npos) break;
            end += 8;
        } else {
            end += 8;
        }

        string single_table_html = normalized.substr(pos, end - pos);
        single_table_html = removeWordStyles(single_table_html);

        // Парсим одну таблицу.
        Table table = parseSimpleHtml(single_table_html);

        // Добавляем только непустые таблицы.
        if (table.getRowCount() > 0) {
            tables.push_back(table);
        }

        pos = end; // Продолжаем поиск после текущей таблицы
    }

    return tables;
}


/**
 * @brief Полный пайплайн очистки для всех таблиц документа
 */
string TableParser::cleanWordHtml(const string& html) {
    string normalized = normalizeHtml(html);
    string result;

    size_t pos = 0;
    bool first_table = true;

    // Ищем все таблицы в документе.
    while ((pos = normalized.find("<table", pos)) != string::npos) {
        size_t end = findMatchingCloseTag(normalized, "table", pos);
        if (end == string::npos) {
            end = normalized.find("</table>", pos);
            if (end == string::npos) break;
            end += 8;
        } else {
            end += 8;
        }

        string table_html = normalized.substr(pos, end - pos);
        table_html = removeWordStyles(table_html);

        Table table = parseSimpleHtml(table_html);
        string table_output = table.toHtml();

        if (first_table) {
            result = table_output;
            first_table = false;
        } else {
            // Добавляем перенос между таблицами для читаемости
            result += "\n\n" + table_output;
        }

        pos = end; // Продолжаем поиск после текущей таблицы
    }

    return result;
}
