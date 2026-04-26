#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql>
#include <QString>
#include <QSqlError>
#include "Table.h"

/**
 * @class DatabaseManager
 * @brief Управляет подключением, созданием таблиц и экспортом данных в SQLite
 */
class DatabaseManager {
public:
    /// Создаёт менеджер БД.
    DatabaseManager();

    /// Закрывает активное соединение при уничтожении объекта.
    ~DatabaseManager();

    /**
     * @brief Подключается к базе данных (создаёт файл, если его нет)
     * @param dbFileName Имя файла базы данных
     * @return true при успехе
     */
    bool connect(const QString& dbFileName);

    /**
     * @brief Создаёт таблицу для импорта (если ещё не существует)
     * @param tableName Имя таблицы
     * @param columnCount Количество колонок (берётся из таблицы)
     * @return true при успехе
     */
    bool createTable(const QString& tableName, int columnCount);

    /**
     * @brief Экспортирует объект Table в SQLite
     * @param table Объект таблицы из памяти
     * @param tableName Имя целевой таблицы в БД
     * @return true при успехе
     */
    bool importTable(const Table& table, const QString& tableName);

    /**
     * @brief Закрывает соединение с БД
     */
    void close();

    /**
     * @brief Возвращает последнюю ошибку (для вывода в интерфейс)
     */
    QString lastError() const { return m_lastError; }

private:
    QSqlDatabase m_db; ///< Подключение Qt к SQLite.
    QString m_lastError; ///< Текст последней ошибки операции.
};

#endif // DATABASEMANAGER_H
