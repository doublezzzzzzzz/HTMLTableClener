/**
 * @file DatabaseManager.cpp
 * @brief Реализация экспорта таблиц в SQLite
 */

#include "DatabaseManager.h"
#include <QFileInfo>
#include <QDebug>

DatabaseManager::DatabaseManager() {}

DatabaseManager::~DatabaseManager() {
    close();
}

bool DatabaseManager::connect(const QString& dbFileName) {
    // Если подключение к этому файлу уже открыто, переиспользуем его.
    if (m_db.isOpen() && m_db.databaseName() == dbFileName) {
        return true;
    }

    // Закрываем предыдущее соединение.
    if (m_db.isOpen()) {
        m_db.close();
    }

    // Используем встроенный драйвер QSQLITE.
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbFileName);

    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        qDebug() << "DB Connection Error:" << m_lastError;
        return false;
    }

    return true;
}

bool DatabaseManager::createTable(const QString& tableName, int columnCount) {
    // Формируем список колонок: col_1 TEXT, col_2 TEXT, ...
    QString columnsDef;
    for (int i = 1; i <= columnCount; ++i) {
        columnsDef += QString("col_%1 TEXT, ").arg(i);
    }
    // Убираем последнюю запятую и пробел.
    columnsDef.chop(2);

    QString queryStr = QString(
                           "CREATE TABLE IF NOT EXISTS %1 ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "%2)"
                           ).arg(tableName).arg(columnsDef);

    QSqlQuery query(m_db);
    if (!query.exec(queryStr)) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::importTable(const Table& table, const QString& tableName) {
    if (!m_db.isOpen()) {
        m_lastError = "Database is not connected";
        return false;
    }

    size_t rows = table.getRowCount();
    if (rows == 0) return true;

    size_t cols = table.getMaxColumnCount();
    if (cols == 0) return true;

    // Формируем INSERT-запрос с плейсхолдерами.
    QString placeholders, columnNames;
    for (size_t i = 1; i <= cols; ++i) {
        columnNames += QString("col_%1, ").arg(i);
        placeholders += "?, ";
    }
    columnNames.chop(2);
    placeholders.chop(2);

    QString insertQuery = QString("INSERT INTO %1 (%2) VALUES (%3)")
                              .arg(tableName).arg(columnNames).arg(placeholders);

    QSqlQuery query(m_db);
    if (!query.prepare(insertQuery)) {
        m_lastError = query.lastError().text();
        return false;
    }

    m_db.transaction();
    int insertedRows = 0;

    for (size_t r = 0; r < rows; ++r) {
        QStringList rowData;

        for (size_t c = 0; c < cols; ++c) {
            auto cell = table.getCell(r, c);

            // По умолчанию считаем ячейку пустой.
            QString content = "";

            if (cell) {
                // Для ReferenceCell берем данные у целевой (главной) ячейки.
                auto target = cell->getTarget();
                if (target && target != cell) {
                    content = QString::fromStdString(target->getContent());
                } else {
                    // Для RealCell берем ее собственные данные.
                    content = QString::fromStdString(cell->getContent());
                }
            }

            rowData << content;
        }

        // Привязываем значения к параметрам запроса.
        for (int i = 0; i < rowData.size(); ++i) {
            query.bindValue(i, rowData[i]);
        }

        if (!query.exec()) {
            m_db.rollback();
            m_lastError = query.lastError().text();
            return false;
        }
        insertedRows++;
    }

    if (!m_db.commit()) {
        m_db.rollback();
        m_lastError = "Failed to commit transaction";
        return false;
    }

    qDebug() << "✓ Exported" << insertedRows << "rows to" << tableName;
    return true;
}

void DatabaseManager::close() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}
