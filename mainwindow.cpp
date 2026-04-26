/**
 * @file mainwindow.cpp
 * @brief Реализация методов класса MainWindow
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TableParser.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QClipboard>
#include <QApplication>
#include <QShortcut>
#include <QTableWidget>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <vector>

/**
 * @brief Конструктор главного окна
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Подключение сигналов кнопок к обработчикам.
    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::onOpenButtonClicked);
    connect(ui->cleanButton, &QPushButton::clicked, this, &MainWindow::onCleanButtonClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::onSaveButtonClicked);
    connect(ui->copyButton, &QPushButton::clicked, this, &MainWindow::onCopyButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::onClearButtonClicked);

    // Горячие клавиши основных действий.
    ui->openButton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
    ui->cleanButton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    ui->saveButton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    ui->copyButton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));
    ui->clearButton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));


    QShortcut *escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escShortcut, &QShortcut::activated, this, &MainWindow::onClearButtonClicked);

    // Базовая настройка виджета визуализации таблиц.
    if (ui->resultTableWidget) {
        ui->resultTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->resultTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->resultTableWidget->setAlternatingRowColors(true);
        ui->resultTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        ui->resultTableWidget->verticalHeader()->setVisible(false);
        ui->resultTableWidget->horizontalHeader()->setVisible(false);
    }

    ui->statusbar->showMessage("Готово. Откройте HTML файл или вставьте код");

    connect(ui->dbExportButton, &QPushButton::clicked, this, &MainWindow::onExportToDbClicked);
    ui->dbExportButton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E)); // Ctrl+E
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Визуализирует набор таблиц в resultTableWidget
 */
void MainWindow::visualizeTables(const std::vector<Table>& tables) {
    if (!ui->resultTableWidget) return;

    ui->resultTableWidget->clear();
    ui->resultTableWidget->setRowCount(0);
    ui->resultTableWidget->setColumnCount(0);
    if (tables.empty()) return;

    // Вычисляем габариты единой визуальной сетки.
    int totalRows = 0;
    int maxCols = 0;

    for (const auto& t : tables) {
        totalRows += static_cast<int>(t.getRowCount()) + 1; // +1 строка под заголовок
    }
    for (const auto& t : tables) {
        maxCols = std::max(maxCols, static_cast<int>(t.getMaxColumnCount()));
    }
    // Добавляем строки-разделители между таблицами.
    totalRows += static_cast<int>(tables.size()) - 1;

    // Устанавливаем размеры до заполнения данными.
    ui->resultTableWidget->setRowCount(totalRows);
    ui->resultTableWidget->setColumnCount(maxCols);

    int curRow = 0;
    for (size_t t = 0; t < tables.size(); ++t) {
        const auto& table = tables[t];
        int rows = static_cast<int>(table.getRowCount());
        int cols = static_cast<int>(table.getMaxColumnCount());

        // Заголовок текущей таблицы.
        auto* header = new QTableWidgetItem(QString("Таблица %1").arg(t + 1));
        header->setBackground(QColor(200, 220, 255));
        header->setForeground(QColor(0, 0, 150));
        header->setTextAlignment(Qt::AlignCenter);
        header->setFlags(Qt::NoItemFlags); // Не выделяется, не редактируется
        ui->resultTableWidget->setItem(curRow, 0, header);
        if (maxCols > 1) ui->resultTableWidget->setSpan(curRow, 0, 1, maxCols);
        ui->resultTableWidget->setRowHeight(curRow, 28);
        curRow++;

        // Данные таблицы.
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                auto cell = table.getCell(r, c);
                if (!cell) continue;

                // Пропускаем ReferenceCell: область уже покрыта главной ячейкой.
                if (cell->getTarget() && cell->getTarget() != cell) continue;

                auto* item = new QTableWidgetItem(QString::fromStdString(cell->getContent()));
                item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
                ui->resultTableWidget->setItem(curRow, c, item);

                int rs = cell->getRowspan();
                int cs = cell->getColspan();

                // Применяем span только для ячеек больше 1x1.
                if ((rs > 1 || cs > 1) && (curRow + rs <= totalRows) && (c + cs <= maxCols)) {
                    ui->resultTableWidget->setSpan(curRow, c, rs, cs);
                }
            }
            // Явно задаем высоту, чтобы строка не схлопывалась из-за span.
            ui->resultTableWidget->setRowHeight(curRow, 30);
            curRow++;
        }

        // Разделитель между таблицами (кроме последней).
        if (t < tables.size() - 1) {
            auto* sep = new QTableWidgetItem("");
            sep->setFlags(Qt::NoItemFlags);
            ui->resultTableWidget->setItem(curRow, 0, sep);
            if (maxCols > 1) ui->resultTableWidget->setSpan(curRow, 0, 1, maxCols);
            ui->resultTableWidget->setRowHeight(curRow, 12);
            curRow++;
        }
    }

    // Финальная настройка визуального вида.
    ui->resultTableWidget->horizontalHeader()->setVisible(false);
    ui->resultTableWidget->verticalHeader()->setVisible(false);
    ui->resultTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->resultTableWidget->setAlternatingRowColors(true);
    ui->resultTableWidget->setShowGrid(true);

    // resizeRowsToContents() не используем: ломает разметку merged ячеек.
}

/**
 * @brief Обработчик кнопки "Открыть"
 */
void MainWindow::onOpenButtonClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Открыть HTML файл", "",
                                                    "HTML файлы (*.html *.htm);;Все файлы (*.*)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось открыть файл: " + file.errorString());
        return;
    }

    QByteArray rawContent = file.readAll();
    file.close();

    QString content;

    // Пытаемся определить кодировку по meta-тегу.
    QString preview = QString::fromLatin1(rawContent.left(2000)).toLower();

    if (preview.contains("charset=windows-1251") ||
        preview.contains("windows-1251")) {

        content = QString::fromUtf8(rawContent);
        // При некорректных символах пробуем локальную кодировку.
        if (content.contains(QChar::ReplacementCharacter)) {
            content = QString::fromLocal8Bit(rawContent);
        }
    } else {
        content = QString::fromUtf8(rawContent);
        if (content.contains(QChar::ReplacementCharacter)) {
            content = QString::fromLatin1(rawContent);
        }
    }

    ui->sourceTextEdit->setPlainText(content);
    ui->statusbar->showMessage(QString("Файл загружен: %1").arg(QFileInfo(fileName).fileName()));
}

/**
 * @brief Обработчик кнопки "Очистить HTML"
 */
void MainWindow::onCleanButtonClicked()
{
    QString source = ui->sourceTextEdit->toPlainText();

    if (source.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение",
                             "Исходное поле пустое.\nОткройте файл или вставьте HTML код.");
        return;
    }

    try {
        ui->statusbar->showMessage("Обработка таблиц...");
        ui->cleanButton->setEnabled(false);
        QApplication::processEvents();

        std::string htmlContent = source.toStdString();
        std::string cleanedHtml = TableParser::cleanWordHtml(htmlContent);

        if (cleanedHtml.empty()) {
            QMessageBox::warning(this, "Предупреждение",
                                 "Таблицы не найдены в HTML.\nУбедитесь, что файл содержит таблицы.");
            ui->statusbar->showMessage("Таблицы не найдены");
            ui->cleanButton->setEnabled(true);
            return;
        }

        // Парсим все таблицы.
        currentTables = TableParser::parseMultipleTables(cleanedHtml);

        if (currentTables.empty()) {
            QMessageBox::warning(this, "Предупреждение",
                                 "Не удалось распарсить таблицы.");
            ui->statusbar->showMessage("Ошибка парсинга");
            ui->cleanButton->setEnabled(true);
            return;
        }

        // Обновляем табличную визуализацию.
        visualizeTables(currentTables);

        // Обновляем текстовый результат.
        updateResultDisplay();

        // Считаем общее количество строк во всех таблицах.
        size_t totalRows = 0;
        for (const auto& table : currentTables) {
            totalRows += table.getRowCount();
        }

        ui->statusbar->showMessage(
            QString("Обработано таблиц: %1, Всего строк: %2")
                .arg(currentTables.size())
                .arg(totalRows)
            );

        ui->cleanButton->setEnabled(true);

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка",
                              QString("Ошибка при обработке:\n%1").arg(e.what()));
        ui->statusbar->showMessage("Ошибка при обработке");
        ui->cleanButton->setEnabled(true);
    }
}

void MainWindow::updateResultDisplay() {
    std::string resultHtml;

    for (size_t i = 0; i < currentTables.size(); ++i) {
        if (i > 0) {
            resultHtml += "\n\n"; // Разделитель между таблицами.
        }
        resultHtml += currentTables[i].toHtml();
    }

    ui->resultTextEdit->setPlainText(QString::fromStdString(resultHtml));
}
/**
 * @brief Обработчик кнопки "Сохранить"
 */
void MainWindow::onSaveButtonClicked()
{
    if (ui->resultTextEdit->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Предупреждение",
                             "Нет данных для сохранения.\nСначала очистите HTML таблицу.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Сохранить очищенный HTML", "",
                                                    "HTML файлы (*.html);;Все файлы (*.*)");
    if (fileName.isEmpty()) return;

    if (!fileName.endsWith(".html", Qt::CaseInsensitive) &&
        !fileName.endsWith(".htm", Qt::CaseInsensitive)) {
        fileName += ".html";
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка",
                             "Не удалось сохранить файл:\n" + file.errorString());
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << ui->resultTextEdit->toPlainText();
    file.close();

    ui->statusbar->showMessage(QString("Файл сохранен: %1").arg(QFileInfo(fileName).fileName()));
}

/**
 * @brief Обработчик кнопки "Копировать"
 */
void MainWindow::onCopyButtonClicked()
{
    if (ui->resultTextEdit->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Предупреждение",
                             "Нет данных для копирования.\nСначала очистите HTML таблицу.");
        return;
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->resultTextEdit->toPlainText());

    QString text = ui->resultTextEdit->toPlainText();
    int lines = text.count('\n') + 1;
    ui->statusbar->showMessage(
        QString("Скопировано: %1 символов, %2 строк").arg(text.length()).arg(lines),
        3000
        );
}

/**
 * @brief Обработчик кнопки "Очистить поля"
 */
void MainWindow::onClearButtonClicked()
{
    bool hasData = !ui->sourceTextEdit->toPlainText().isEmpty() ||
                   !ui->resultTextEdit->toPlainText().isEmpty();

    if (hasData) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "Очистка полей", "Очистить все поля?",
            QMessageBox::Yes | QMessageBox::No);

        if (reply != QMessageBox::Yes) return;
    }

    clearAllFields();
}

/**
 * @brief Полная очистка всех полей и данных
 */
void MainWindow::clearAllFields() {
    ui->sourceTextEdit->clear();
    ui->resultTextEdit->clear();

    if (ui->resultTableWidget) {
        ui->resultTableWidget->clear();
        ui->resultTableWidget->setRowCount(0);
        ui->resultTableWidget->setColumnCount(0);
    }

    currentTables.clear();
    ui->statusbar->showMessage("Поля очищены");
}

/**
 * @brief Экспорт текущих таблиц в SQLite
 */
void MainWindow::onExportToDbClicked()
{
    if (currentTables.empty()) {
        QMessageBox::warning(this, "Предупреждение",
                             "Нет данных для экспорта.\nСначала обработайте HTML.");
        return;
    }

    // Диалог выбора файла SQLite.
    QString dbFile = QFileDialog::getSaveFileName(this,
                                                  "Сохранить базу данных",
                                                  "eri_database.sqlite",
                                                  "SQLite Database (*.sqlite *.db);;All Files (*)");
    if (dbFile.isEmpty()) return;

    ui->statusbar->showMessage("Экспорт в БД...");
    QApplication::processEvents();

    // Подключение к БД.
    if (!m_dbManager.connect(dbFile)) {
        QMessageBox::critical(this, "Ошибка БД", m_dbManager.lastError());
        ui->statusbar->showMessage("Ошибка подключения к БД");
        return;
    }

    // Экспортируем каждую HTML-таблицу в отдельную таблицу БД.
    int successCount = 0;
    for (size_t i = 0; i < currentTables.size(); ++i) {
        QString tableName = QString("table_%1").arg(i + 1);

        // Создаем структуру таблицы под нужное количество колонок.
        int cols = static_cast<int>(currentTables[i].getMaxColumnCount());
        if (!m_dbManager.createTable(tableName, cols)) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Не удалось создать таблицу %1:\n%2")
                                     .arg(tableName).arg(m_dbManager.lastError()));
            continue;
        }

        // Импортируем данные таблицы.
        if (m_dbManager.importTable(currentTables[i], tableName)) {
            successCount++;
        } else {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Не удалось импортировать %1:\n%2")
                                     .arg(tableName).arg(m_dbManager.lastError()));
        }
    }

    if (successCount > 0) {
        QMessageBox::information(this, "Успех",
                                 QString("Успешно экспортировано таблиц: %1\nФайл: %2")
                                     .arg(successCount).arg(QFileInfo(dbFile).fileName()));
        ui->statusbar->showMessage(QString("Экспорт завершён: %1 таблиц").arg(successCount), 3000);
    } else {
        ui->statusbar->showMessage("Экспорт не выполнен");
    }
}
