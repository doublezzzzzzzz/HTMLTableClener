/**
 * @file Cell.h
 * @brief Иерархия классов для ячеек таблицы с поддержкой ссылок для merged cells
 */

#ifndef CELL_H
#define CELL_H

#include <string>
#include <memory>

/**
 * @class AbstractCell
 * @brief Абстрактный базовый класс для всех типов ячеек
 */
class AbstractCell {
public:
    virtual ~AbstractCell() = default;

    virtual std::string getContent() const = 0;
    virtual bool getIsHeader() const = 0;
    virtual int getRowspan() const = 0;   ///< Виртуальный rowspan для сетки
    virtual int getColspan() const = 0;   ///< Виртуальный colspan для сетки

    /// Возвращает целевую ячейку (для RealCell — саму себя).
    virtual std::shared_ptr<AbstractCell> getTarget() = 0;
};

/**
 * @class RealCell
 * @brief Ячейка с реальными данными (создается при парсинге <td>/<th>)
 */
class RealCell : public AbstractCell, public std::enable_shared_from_this<RealCell> {
private:
    std::string content;
    int rowspan;   ///< Исходный rowspan из HTML
    int colspan;   ///< Исходный colspan из HTML
    bool isHeader;

public:
    RealCell(const std::string& content, int rowspan = 1, int colspan = 1, bool isHeader = false)
        : content(content), rowspan(rowspan), colspan(colspan), isHeader(isHeader) {}

    std::string getContent() const override { return content; }
    bool getIsHeader() const override { return isHeader; }
    int getRowspan() const override { return rowspan; }
    int getColspan() const override { return colspan; }

    /// Возвращает shared_ptr на текущий объект.
    std::shared_ptr<AbstractCell> getTarget() override {
        return shared_from_this();
    }

    /// Служебные сеттеры, используются при построении сетки.
    void setRowspan(int rs) { rowspan = rs; }
    void setColspan(int cs) { colspan = cs; }
};

/**
 * @class ReferenceCell
 * @brief Ячейка-ссылка на другую ячейку (для merged cells в сетке)
 *
 * Не хранит данные самостоятельно, а делегирует вызовы целевой ячейке.
 * Для виртуальной сетки всегда занимает одну позицию (1x1).
 */
class ReferenceCell : public AbstractCell, public std::enable_shared_from_this<ReferenceCell> {
private:
    std::shared_ptr<AbstractCell> target;  ///< Ссылка на целевую ячейку
    bool isHeader;                         ///< Кэш признака заголовка

public:
    explicit ReferenceCell(std::shared_ptr<AbstractCell> targetCell)
        : target(targetCell), isHeader(targetCell->getIsHeader()) {}

    std::string getContent() const override {
        return target ? target->getContent() : "";
    }

    bool getIsHeader() const override { return isHeader; }

    /// Для сетки ссылка всегда имеет размер 1x1.
    int getRowspan() const override { return 1; }
    int getColspan() const override { return 1; }

    /// Возвращает целевую ячейку.
    std::shared_ptr<AbstractCell> getTarget() override { return target; }
};

/// Удобный алиас для указателя на ячейку.
using CellPtr = std::shared_ptr<AbstractCell>;

#endif // CELL_H
