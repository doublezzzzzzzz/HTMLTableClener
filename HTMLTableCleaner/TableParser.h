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
 * Все методы класса статические, экземпляры класса не создаются.
 * Предоставляет функции для извлечения таблиц из Word HTML,
 * очистки от форматирования и парсинга в структуру Table.
 */
class TableParser {
public:
    /**
     * @brief Парсит простой HTML-код таблицы в объект Table
     * @param html HTML-код таблицы (ожидается уже очищенный)
     * @return Объект Table с распарсенными данными
     */
    static Table parseSimpleHtml(const std::string& html);

    /**
     * @brief Извлекает таблицу из полного HTML-кода Word-документа
     * @param html Полный HTML-код документа
     * @return HTML-код только таблицы или пустая строка
     */
    static std::string extractTableFromWordHtml(const std::string& html);

    /**
     * @brief Комплексная очистка HTML-кода Word-документа и извлечение таблицы
     * @param html Полный HTML-код Word-документа
     * @return Чистый HTML-код таблицы
     * 
     * Главный метод для очистки Word HTML. Объединяет извлечение таблицы,
     * парсинг и преобразование обратно в HTML.
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
