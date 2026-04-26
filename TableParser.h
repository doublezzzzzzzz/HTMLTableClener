/**
 * @file TableParser.h
 * @brief Определение класса TableParser - парсера и очистителя HTML-таблиц
 */

#ifndef TABLEPARSER_H
#define TABLEPARSER_H

#include "Table.h"
#include <string>

/**
 * @class TableParser
 * @brief Утилитарный класс для парсинга и очистки HTML-таблиц
 * 
 * Класс содержит только статические методы. Выполняет нормализацию HTML,
 * очистку Word-специфичного мусора и преобразование таблиц в модель Table.
 */
class TableParser {
public:
    /**
     * @brief Парсит одну таблицу в модель Table
     * @param html HTML одной таблицы
     * @return Таблица во внутреннем представлении
     */
    static Table parseSimpleHtml(const std::string& html);

    /**
     * @brief Парсит все таблицы, найденные в HTML
     * @param html HTML документа
     * @return Список распарсенных таблиц
     */
    static std::vector<Table> parseMultipleTables(const std::string& html);

    /**
     * @brief Извлекает первую таблицу из HTML
     * @param html Полный HTML-код документа
     * @return HTML первой таблицы или пустая строка
     */
    static std::string extractTableFromWordHtml(const std::string& html);

    /**
     * @brief Полный пайплайн очистки для всех таблиц в HTML
     * @param html Полный HTML-код Word-документа
     * @return Чистый HTML-код всех найденных таблиц
     */
    static std::string cleanWordHtml(const std::string& html);

private:
    static std::string cleanCellContent(const std::string& content);
    static int extractRowspan(const std::string& html, size_t pos);
    static int extractColspan(const std::string& html, size_t pos);
    static std::string removeWordStyles(const std::string& html);
    static std::string normalizeHtml(const std::string& html);
};

#endif // TABLEPARSER_H
