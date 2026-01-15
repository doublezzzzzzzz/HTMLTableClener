/**
 * @file TableParser.cpp
 * @brief Реализация методов класса TableParser и вспомогательных функций
 */

#include "TableParser.h"
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;

/**
 * @brief Находит соответствующий закрывающий тег с учетом вложенности
 * @param html HTML-строка для поиска
 * @param tag Имя тега (без угловых скобок, например "table", "tr")
 * @param start_pos Позиция начала поиска
 * @return Позиция закрывающего тега или string::npos, если не найден
 * 
 * Отслеживает уровень вложенности тегов для корректного поиска
 * закрывающего тега при наличии вложенных тегов с тем же именем.
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
 * @param html HTML-строка
 * @param attr_name Имя атрибута (например "rowspan", "colspan")
 * @param pos Позиция начала поиска
 * @return Значение атрибута или "1" по умолчанию, если атрибут не найден
 * 
 * Обрабатывает пробелы, кавычки и табуляции вокруг знака равенства.
 */
string extractAttribute(const string& html, const string& attr_name, size_t pos) {
    size_t attr_start = html.find(attr_name, pos);
    if (attr_start == string::npos) {
        return "1";
    }

    size_t equals_pos = html.find('=', attr_start);
    if (equals_pos == string::npos) {
        return "1";
    }

    size_t value_start = equals_pos + 1;
    while (value_start < html.length() &&
           (html[value_start] == ' ' || html[value_start] == '\t' ||
            html[value_start] == '\"' || html[value_start] == '\'')) {
        value_start++;
    }

    if (value_start >= html.length()) {
        return "1";
    }

    size_t value_end = value_start;
    while (value_end < html.length() &&
           html[value_end] != ' ' && html[value_end] != '\t' &&
           html[value_end] != '>' && html[value_end] != '\"' && html[value_end] != '\'') {
        value_end++;
    }

    return html.substr(value_start, value_end - value_start);
}

/**
 * @brief Удаляет теги форматирования из текста, сохраняя текстовое содержимое
 * @param text Текст с HTML-тегами форматирования
 * @return Текст без тегов форматирования
 * 
 * Обрабатывает следующие теги: <b>, <strong>, <i>, <em>, <u>, <ins>,
 * <s>, <strike>, <del>, <span>, <font> (в различных регистрах).
 * Рекурсивно обрабатывает вложенные теги.
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

            if (tag_pair.first.find('<') != string::npos) {
                tag_end = result.find('>', pos);
                if (tag_end == string::npos) break;
            }

            size_t close_tag_start = result.find(tag_pair.second, tag_end);
            if (close_tag_start == string::npos) {
                result.erase(pos, tag_end - pos + 1);
            } else {
                string inner_content = result.substr(tag_end + 1, close_tag_start - tag_end - 1);
                inner_content = removeFormattingTags(inner_content);
                result.replace(pos, close_tag_start - pos + tag_pair.second.length(), inner_content);
                pos += inner_content.length();
            }
        }
    }

    return result;
}

/**
 * @brief Комплексная очистка содержимого ячейки таблицы
 * @param content Исходное содержимое ячейки
 * @return Очищенное содержимое
 * 
 * Выполняет следующие этапы обработки:
 * 1. Удаление тегов форматирования
 * 2. Обработка параграфов (<p>...</p>) с сохранением структуры
 * 3. Удаление HTML-сущностей (&nbsp;, &amp; и т.д.)
 * 4. Удаление пустых параграфов
 * 5. Нормализация множественных пробелов
 * 6. Обрезка пробелов по краям
 */
string TableParser::cleanCellContent(const string& content) {
    string result = content;

    // 1. Удаляем все теги форматирования (жирный, курсив, подчеркивание и т.д.)
    result = removeFormattingTags(result);

    // 2. Обработка параграфов - сохраняем структуру, но очищаем внутреннее содержимое
    vector<pair<size_t, size_t>> paragraph_ranges;

    // Находим все параграфы
    size_t p_pos = 0;
    while ((p_pos = result.find("<p", p_pos)) != string::npos) {
        size_t p_end = result.find('>', p_pos);
        if (p_end == string::npos) break;

        size_t paragraph_start = p_end + 1;

        // Ищем закрывающий </p>
        size_t close_p = result.find("</p>", p_end);
        if (close_p != string::npos) {
            paragraph_ranges.push_back({p_pos, close_p + 4}); // Сохраняем позиции <p>...</p>
            p_pos = close_p + 4;
        } else {
            // Если нет закрывающего тега, ищем следующий открывающий тег или конец
            p_pos = p_end + 1;
        }
    }

    // 3. Обрабатываем содержимое вне параграфов (удаляем все теги кроме <br>)
    if (!paragraph_ranges.empty()) {
        string processed;
        size_t last_pos = 0;

        for (size_t i = 0; i < paragraph_ranges.size(); i++) {
            size_t para_start = paragraph_ranges[i].first;
            size_t para_end = paragraph_ranges[i].second;

            // Обрабатываем текст до параграфа
            string before_para = result.substr(last_pos, para_start - last_pos);

            // Удаляем все теги из текста перед параграфом
            string cleaned_before;
            bool in_tag = false;
            for (char c : before_para) {
                if (c == '<') {
                    in_tag = true;
                } else if (c == '>') {
                    in_tag = false;
                } else if (!in_tag) {
                    cleaned_before += c;
                }
            }
            processed += cleaned_before;

            // Извлекаем параграф
            string paragraph = result.substr(para_start, para_end - para_start);

            // Удаляем атрибуты из <p> тега
            size_t p_tag_end = paragraph.find('>');
            if (p_tag_end != string::npos) {
                string clean_p_tag = "<p>";
                string para_content = paragraph.substr(p_tag_end + 1, paragraph.length() - p_tag_end - 5); // -5 для </p>

                // Очищаем содержимое параграфа от форматирования
                para_content = removeFormattingTags(para_content);

                // Удаляем <br> теги, заменяя их на пробелы
                size_t br_pos = 0;
                while ((br_pos = para_content.find("<br", br_pos)) != string::npos) {
                    size_t br_end = para_content.find('>', br_pos);
                    if (br_end != string::npos) {
                        para_content.replace(br_pos, br_end - br_pos + 1, " ");
                        br_pos += 1;
                    } else {
                        br_pos++;
                    }
                }

                // Удаляем лишние пробелы в содержимом параграфа
                string clean_para_content;
                bool last_was_space = false;
                for (char c : para_content) {
                    if (isspace(static_cast<unsigned char>(c))) {
                        if (!last_was_space && !clean_para_content.empty()) {
                            clean_para_content += ' ';
                            last_was_space = true;
                        }
                    } else {
                        clean_para_content += c;
                        last_was_space = false;
                    }
                }

                // Обрезаем пробелы по краям
                while (!clean_para_content.empty() && isspace(static_cast<unsigned char>(clean_para_content.front()))) {
                    clean_para_content.erase(0, 1);
                }
                while (!clean_para_content.empty() && isspace(static_cast<unsigned char>(clean_para_content.back()))) {
                    clean_para_content.pop_back();
                }

                // Если содержимое не пустое, добавляем параграф
                if (!clean_para_content.empty()) {
                    processed += clean_p_tag + clean_para_content + "</p>";
                }
            }

            last_pos = para_end;
        }

        // Обрабатываем текст после последнего параграфа
        if (last_pos < result.length()) {
            string after_last = result.substr(last_pos);
            string cleaned_after;
            bool in_tag = false;
            for (char c : after_last) {
                if (c == '<') {
                    in_tag = true;
                } else if (c == '>') {
                    in_tag = false;
                } else if (!in_tag) {
                    cleaned_after += c;
                }
            }
            processed += cleaned_after;
        }

        result = processed;
    } else {
        // Если нет параграфов, просто удаляем все теги
        string cleaned;
        bool in_tag = false;
        for (char c : result) {
            if (c == '<') {
                in_tag = true;
            } else if (c == '>') {
                in_tag = false;
            } else if (!in_tag) {
                cleaned += c;
            }
        }
        result = cleaned;
    }

    // 4. Удаляем HTML-сущности
    vector<pair<string, string>> html_entities = {
        {"&nbsp;", " "}, {"&amp;", "&"}, {"&lt;", "<"}, {"&gt;", ">"},
        {"&quot;", "\""}, {"&apos;", "'"}, {"&#39;", "'"}, {"&#34;", "\""},
        {"&#160;", " "}, {"&ensp;", " "}, {"&emsp;", " "}
    };

    for (const auto& entity : html_entities) {
        size_t pos = 0;
        while ((pos = result.find(entity.first, pos)) != string::npos) {
            result.replace(pos, entity.first.length(), entity.second);
            pos += entity.second.length();
        }
    }

    // 5. Удаляем пустые параграфы и параграфы с пробелами
    size_t empty_p_pos = 0;
    while ((empty_p_pos = result.find("<p></p>", empty_p_pos)) != string::npos) {
        result.erase(empty_p_pos, 7);
    }

    // Также удаляем <p> </p> (с пробелом внутри)
    size_t space_p_pos = 0;
    while ((space_p_pos = result.find("<p> ", space_p_pos)) != string::npos) {
        if (space_p_pos + 6 < result.length() && result.substr(space_p_pos + 4, 4) == "</p>") {
            result.erase(space_p_pos, 8); // <p> </p>
        } else {
            space_p_pos++;
        }
    }

    // 6. Удаляем множественные пробелы
    string final_result;
    bool last_was_space = false;
    for (char c : result) {
        if (isspace(static_cast<unsigned char>(c))) {
            if (!last_was_space && !final_result.empty()) {
                final_result += ' ';
                last_was_space = true;
            }
        } else {
            final_result += c;
            last_was_space = false;
        }
    }

    // 7. Обрезаем пробелы по краям
    while (!final_result.empty() && isspace(static_cast<unsigned char>(final_result.front()))) {
        final_result.erase(0, 1);
    }
    while (!final_result.empty() && isspace(static_cast<unsigned char>(final_result.back()))) {
        final_result.pop_back();
    }

    return final_result;
}

/**
 * @brief Удаляет Word-специфичные стили и атрибуты из HTML-кода
 * @param html HTML-код
 * @return HTML без Word-стилей
 * 
 * Удаляет атрибуты class, style и XML namespace declarations,
 * которые добавляет Microsoft Word при экспорте в HTML.
 */
string TableParser::removeWordStyles(const string& html) {
    string result = html;

    // Удаляем классы MSO
    size_t class_pos = 0;
    while ((class_pos = result.find("class=\"", class_pos)) != string::npos) {
        size_t quote_end = result.find('"', class_pos + 7);
        if (quote_end == string::npos) break;

        result.erase(class_pos, quote_end - class_pos + 1);
    }

    // Удаляем стили
    size_t style_pos = 0;
    while ((style_pos = result.find("style=\"", style_pos)) != string::npos) {
        size_t quote_end = result.find('"', style_pos + 7);
        if (quote_end != string::npos) {
            result.erase(style_pos, quote_end - style_pos + 1);
        } else {
            style_pos += 7;
        }
    }

    // Удаляем namespace declarations
    vector<string> namespaces = {
        "xmlns:v=", "xmlns:o=", "xmlns:w=", "xmlns:m=",
        "v:lang=", "o:lang=", "lang=", "xmlns="
    };

    for (const auto& ns : namespaces) {
        size_t pos = 0;
        while ((pos = result.find(ns, pos)) != string::npos) {
            size_t quote_start = result.find('"', pos);
            if (quote_start == string::npos) break;
            size_t quote_end = result.find('"', quote_start + 1);
            if (quote_end == string::npos) break;

            result.erase(pos, quote_end - pos + 1);
        }
    }

    return result;
}

/**
 * @brief Нормализация HTML-кода (удаление ненужных элементов, сохранение структуры таблицы)
 * @param html Исходный HTML
 * @return Нормализованный HTML
 * 
 * Удаляет HTML-комментарии, CDATA секции и все теги, кроме разрешенных
 * (<table>, <tr>, <td>, <th>, <p>, <br> и их закрывающие теги).
 * Удаляет пустые теги <tbody>.
 */
string TableParser::normalizeHtml(const string& html) {
    string result = html;

    // Удаляем комментарии
    size_t comment_start = 0;
    while ((comment_start = result.find("<!--", comment_start)) != string::npos) {
        size_t comment_end = result.find("-->", comment_start);
        if (comment_end == string::npos) break;

        result.erase(comment_start, comment_end - comment_start + 3);
    }

    // Удаляем CDATA
    size_t cdata_start = 0;
    while ((cdata_start = result.find("<![CDATA[", cdata_start)) != string::npos) {
        size_t cdata_end = result.find("]]>", cdata_start);
        if (cdata_end == string::npos) break;

        result.erase(cdata_start, cdata_end - cdata_start + 3);
    }

    // Удаляем все теги кроме разрешенных: table, tr, td, th, p, br и их закрывающих
    // Но сохраняем теги с атрибутами для table, td, th (colspan, rowspan)
    string temp;
    size_t pos = 0;

    while (pos < result.length()) {
        if (result[pos] == '<') {
            // Нашли начало тега
            size_t tag_end = result.find('>', pos);
            if (tag_end == string::npos) {
                temp += result.substr(pos);
                break;
            }

            string tag = result.substr(pos, tag_end - pos + 1);
            string tag_lower = tag;
            transform(tag_lower.begin(), tag_lower.end(), tag_lower.begin(), ::tolower);

            // Проверяем, является ли тег разрешенным
            bool is_allowed = false;

            // Проверяем открывающие теги
            if (tag_lower.find("<table") == 0 ||
                tag_lower.find("<tr") == 0 ||
                tag_lower.find("<td") == 0 ||
                tag_lower.find("<th") == 0 ||
                tag_lower.find("<p") == 0 ||
                tag_lower.find("<br") == 0 ||
                tag_lower.find("</table>") == 0 ||
                tag_lower.find("</tr>") == 0 ||
                tag_lower.find("</td>") == 0 ||
                tag_lower.find("</th>") == 0 ||
                tag_lower.find("</p>") == 0) {
                is_allowed = true;
            }

            // Также разрешаем тег <tbody> который может быть добавлен браузером
            if (tag_lower.find("<tbody") == 0 || tag_lower.find("</tbody>") == 0) {
                is_allowed = true;
            }

            if (is_allowed) {
                temp += tag;
            } else {
                // Неразрешенный тег - заменяем его содержимым если оно есть
                // Ищем содержимое до следующего тега
                size_t next_tag = result.find('<', tag_end + 1);
                if (next_tag != string::npos) {
                    string content = result.substr(tag_end + 1, next_tag - tag_end - 1);
                    temp += content;
                    pos = next_tag;
                    continue;
                }
            }

            pos = tag_end + 1;
        } else {
            // Просто текст, добавляем его
            temp += result[pos];
            pos++;
        }
    }

    result = temp;

    // Удаляем пустые теги <tbody>
    size_t tbody_pos = 0;
    while ((tbody_pos = result.find("<tbody>", tbody_pos)) != string::npos) {
        size_t tbody_end = result.find("</tbody>", tbody_pos);
        if (tbody_end != string::npos) {
            // Извлекаем содержимое tbody
            string tbody_content = result.substr(tbody_pos + 7, tbody_end - tbody_pos - 7);
            // Заменяем <tbody>...</tbody> на его содержимое
            result.replace(tbody_pos, tbody_end - tbody_pos + 8, tbody_content);
        } else {
            tbody_pos += 7;
        }
    }

    return result;
}

/**
 * @brief Извлекает таблицу из полного HTML-кода Word-документа
 * @param html Полный HTML-код документа
 * @return HTML-код только таблицы или пустая строка, если таблица не найдена
 * 
 * Нормализует HTML, находит первый тег <table>, извлекает его содержимое
 * и удаляет Word-специфичные стили.
 */
string TableParser::extractTableFromWordHtml(const string& html) {
    cout << "HTML size: " << html.length() << " characters" << endl;

    string normalized = normalizeHtml(html);

    size_t start = normalized.find("<table");
    if (start == string::npos) {
        cout << "No table found" << endl;
        return "";
    }

    size_t end = findMatchingCloseTag(normalized, "table", start);
    if (end == string::npos) {
        end = normalized.find("</table>", start);
        if (end == string::npos) {
            cout << "No matching closing table tag found" << endl;
            return "";
        }
        end += 8;
    } else {
        end += 8;
    }

    string table_html = normalized.substr(start, end - start);
    cout << "Extracted table size: " << table_html.length() << " characters" << endl;

    table_html = removeWordStyles(table_html);

    return table_html;
}

/**
 * @brief Извлекает значение атрибута rowspan из HTML-тега
 * @param html HTML-строка
 * @param pos Позиция начала поиска
 * @return Значение rowspan или 1 по умолчанию
 */
int TableParser::extractRowspan(const string& html, size_t pos) {
    string rowspan = extractAttribute(html, "rowspan", pos);
    if (rowspan == "1") {
        return 1;
    }

    try {
        return stoi(rowspan);
    } catch (...) {
        return 1;
    }
}

/**
 * @brief Извлекает значение атрибута colspan из HTML-тега
 * @param html HTML-строка
 * @param pos Позиция начала поиска
 * @return Значение colspan или 1 по умолчанию
 */
int TableParser::extractColspan(const string& html, size_t pos) {
    string colspan = extractAttribute(html, "colspan", pos);
    if (colspan == "1") {
        return 1;
    }

    try {
        return stoi(colspan);
    } catch (...) {
        return 1;
    }
}

/**
 * @brief Парсит простой HTML-код таблицы в объект Table
 * @param html HTML-код таблицы (ожидается уже очищенный)
 * @return Объект Table с распарсенными данными
 * 
 * Находит все строки (<tr>), для каждой строки извлекает ячейки (<td>, <th>),
 * извлекает атрибуты (rowspan, colspan) и содержимое, очищает содержимое
 * ячеек и создает объекты Cell для построения структуры таблицы.
 */
Table TableParser::parseSimpleHtml(const string& html) {
    Table table;

    cout << "=== START PARSING ===" << endl;

    if (html.empty()) {
        cout << "Empty HTML provided" << endl;
        return table;
    }

    string working_html = html;

    // Парсим строки
    size_t pos = 0;
    int row_count = 0;

    while ((pos = working_html.find("<tr", pos)) != string::npos) {
        row_count++;

        size_t tr_end = findMatchingCloseTag(working_html, "tr", pos);
        if (tr_end == string::npos) {
            tr_end = working_html.find("</tr>", pos);
            if (tr_end == string::npos) {
                break;
            }
            tr_end += 5;
        } else {
            tr_end += 5;
        }

        string row_html = working_html.substr(pos, tr_end - pos);

        vector<Cell> row_cells;
        size_t cell_pos = 0;

        while (true) {
            size_t td_pos = row_html.find("<td", cell_pos);
            size_t th_pos = row_html.find("<th", cell_pos);

            size_t cell_start;
            bool is_header = false;

            if (td_pos != string::npos && th_pos != string::npos) {
                if (td_pos < th_pos) {
                    cell_start = td_pos;
                } else {
                    cell_start = th_pos;
                    is_header = true;
                }
            } else if (td_pos != string::npos) {
                cell_start = td_pos;
            } else if (th_pos != string::npos) {
                cell_start = th_pos;
                is_header = true;
            } else {
                break;
            }

            string tag_name = is_header ? "th" : "td";

            size_t cell_end = findMatchingCloseTag(row_html, tag_name, cell_start);
            if (cell_end == string::npos) {
                cell_end = row_html.find("</" + tag_name + ">", cell_start);
                if (cell_end == string::npos) {
                    break;
                }
                cell_end += tag_name.length() + 3;
            } else {
                cell_end += tag_name.length() + 3;
            }

            string cell_html = row_html.substr(cell_start, cell_end - cell_start);

            // Извлекаем атрибуты
            int rowspan = extractRowspan(cell_html, 0);
            int colspan = extractColspan(cell_html, 0);

            // Извлекаем и очищаем содержимое
            size_t content_start = cell_html.find('>');
            if (content_start != string::npos) {
                content_start++;
                size_t content_end = cell_html.rfind('<');
                if (content_end > content_start) {
                    string content = cell_html.substr(content_start, content_end - content_start);
                    content = cleanCellContent(content);

                    row_cells.push_back(Cell(content, rowspan, colspan, is_header));
                } else {
                    row_cells.push_back(Cell("", rowspan, colspan, is_header));
                }
            } else {
                row_cells.push_back(Cell("", rowspan, colspan, is_header));
            }

            cell_pos = cell_end;
        }

        if (!row_cells.empty()) {
            table.addRow(row_cells);
            cout << "Added row " << row_count << " with " << row_cells.size() << " cells" << endl;
        }

        pos = tr_end;
    }

    cout << "=== END PARSING ===" << endl;
    cout << "Parsed " << table.getRows().size() << " rows total" << endl;

    return table;
}

/**
 * @brief Комплексная очистка HTML-кода Word-документа и извлечение таблицы
 * @param html Полный HTML-код Word-документа
 * @return Чистый HTML-код таблицы или пустая строка, если таблица не найдена
 * 
 * Главный метод для очистки Word HTML. Объединяет извлечение таблицы,
 * парсинг в объект Table и преобразование обратно в HTML.
 */
string TableParser::cleanWordHtml(const string& html) {
    string table_html = extractTableFromWordHtml(html);
    if (table_html.empty()) {
        return "";
    }

    Table table = parseSimpleHtml(table_html);
    return table.toHtml();
}
