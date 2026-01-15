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

/**
 * @brief Конструктор главного окна
 * @param parent Родительский виджет
 * 
 * Инициализирует интерфейс, подключает сигналы кнопок к слотам,
 * настраивает горячие клавиши и устанавливает начальное сообщение в статусной строке.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Подключаем сигналы кнопок к соответствующим слотам
    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::onOpenButtonClicked);
    connect(ui->cleanButton, &QPushButton::clicked, this, &MainWindow::onCleanButtonClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::onSaveButtonClicked);
    connect(ui->copyButton, &QPushButton::clicked, this, &MainWindow::onCopyButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked, this, &MainWindow::onClearButtonClicked);

    // Настраиваем горячие клавиши для всех кнопок
    ui->openButton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));    // Ctrl+O - Открыть
    ui->cleanButton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));   // Ctrl+R - Очистить
    ui->saveButton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));    // Ctrl+S - Сохранить
    ui->copyButton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));    // Ctrl+C - Копировать
    ui->clearButton->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));   // Ctrl+L - Очистить поля

    // Дополнительная горячая клавиша Esc для очистки
    QShortcut *escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escShortcut, &QShortcut::activated, this, &MainWindow::onClearButtonClicked);

    // Настраиваем начальное состояние статусной строки
    ui->statusbar->showMessage("Готово. Откройте HTML файл или вставьте код");
}

/**
 * @brief Деструктор главного окна
 * 
 * Освобождает ресурсы интерфейса.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Обработчик нажатия кнопки "Открыть"
 * 
 * Открывает диалог выбора файла, читает HTML-файл с кодировкой UTF-8
 * и загружает содержимое в текстовое поле исходного HTML.
 * Обновляет статусную строку с именем загруженного файла.
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

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    QString content = in.readAll();
    file.close();

    ui->sourceTextEdit->setPlainText(content);
    ui->statusbar->showMessage(QString("Файл загружен: %1").arg(QFileInfo(fileName).fileName()));
}

/**
 * @brief Обработчик нажатия кнопки "Очистить HTML"
 * 
 * Обрабатывает исходный HTML-код: извлекает таблицу, очищает её от
 * Word-специфичных стилей и форматирования, парсит в объект Table
 * и отображает результат. Обрабатывает ошибки и обновляет статусную строку.
 */
void MainWindow::onCleanButtonClicked()
{
    QString source = ui->sourceTextEdit->toPlainText();

    // Проверяем наличие исходного кода
    if (source.isEmpty()) {
        QMessageBox::warning(this, "Предупреждение",
                             "Исходное поле пустое.\nОткройте файл или вставьте HTML код.");
        return;
    }

    try {
        ui->statusbar->showMessage("Обработка таблицы...");
        ui->cleanButton->setEnabled(false);
        QApplication::processEvents();

        // Преобразуем QString в std::string и очищаем HTML
        std::string htmlContent = source.toStdString();
        std::string cleanedHtml = TableParser::cleanWordHtml(htmlContent);

        // Проверяем, была ли найдена таблица
        if (cleanedHtml.empty()) {
            QMessageBox::warning(this, "Предупреждение",
                                 "Таблица не найдена в HTML.\nУбедитесь, что файл содержит таблицу.");
            ui->statusbar->showMessage("Таблица не найдена");
            ui->cleanButton->setEnabled(true);
            return;
        }

        // Парсим очищенный HTML в объект Table и обновляем отображение
        currentTable = TableParser::parseSimpleHtml(cleanedHtml);
        updateResultDisplay();

        // Обновляем статусную строку с информацией о результате
        ui->statusbar->showMessage(
            QString("Таблица обработана. Строк: %1, Символов: %2")
                .arg(currentTable.getRows().size())
                .arg(ui->resultTextEdit->toPlainText().length())
            );

        ui->cleanButton->setEnabled(true);

    } catch (const std::exception& e) {
        // Обрабатываем исключения при парсинге
        QMessageBox::critical(this, "Ошибка",
                              QString("Ошибка при обработке:\n%1").arg(e.what()));
        ui->statusbar->showMessage("Ошибка при обработке");
        ui->cleanButton->setEnabled(true);
    }
}

/**
 * @brief Обновляет отображение результата в текстовом поле
 * 
 * Преобразует объект currentTable в HTML-код через метод toHtml()
 * и загружает результат в текстовое поле resultTextEdit.
 */
void MainWindow::updateResultDisplay() {
    std::string resultHtml = currentTable.toHtml();
    ui->resultTextEdit->setPlainText(QString::fromStdString(resultHtml));
}

/**
 * @brief Обработчик нажатия кнопки "Сохранить"
 * 
 * Сохраняет очищенный HTML-код в файл. Открывает диалог сохранения файла,
 * автоматически добавляет расширение .html, если не указано, и записывает
 * данные с кодировкой UTF-8.
 */
void MainWindow::onSaveButtonClicked()
{
    // Проверяем наличие данных для сохранения
    if (ui->resultTextEdit->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Предупреждение",
                             "Нет данных для сохранения.\nСначала очистите HTML таблицу.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Сохранить очищенный HTML", "",
                                                    "HTML файлы (*.html);;Все файлы (*.*)");

    if (fileName.isEmpty()) return;

    // Автоматически добавляем расширение .html, если не указано
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

    // Записываем данные с кодировкой UTF-8
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << ui->resultTextEdit->toPlainText();
    file.close();

    ui->statusbar->showMessage(QString("Файл сохранен: %1").arg(QFileInfo(fileName).fileName()));
}

/**
 * @brief Обработчик нажатия кнопки "Копировать"
 * 
 * Копирует очищенный HTML-код в системный буфер обмена.
 * Обновляет статусную строку с информацией о количестве скопированных символов и строк.
 * Сообщение отображается 3 секунды.
 */
void MainWindow::onCopyButtonClicked()
{
    // Проверяем наличие данных для копирования
    if (ui->resultTextEdit->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Предупреждение",
                             "Нет данных для копирования.\nСначала очистите HTML таблицу.");
        return;
    }

    // Копируем текст в буфер обмена
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->resultTextEdit->toPlainText());

    // Обновляем статусную строку с информацией о скопированных данных
    QString text = ui->resultTextEdit->toPlainText();
    int lines = text.count('\n') + 1;
    ui->statusbar->showMessage(
        QString("Скопировано: %1 символов, %2 строк").arg(text.length()).arg(lines),
        3000  // Сообщение отображается 3 секунды
        );
}

/**
 * @brief Обработчик нажатия кнопки "Очистить поля"
 * 
 * Очищает все текстовые поля и данные. Показывает диалог подтверждения,
 * если в полях есть данные. Вызывает clearAllFields() для выполнения очистки.
 */
void MainWindow::onClearButtonClicked()
{
    // Проверяем наличие данных в полях
    bool hasData = !ui->sourceTextEdit->toPlainText().isEmpty() ||
                   !ui->resultTextEdit->toPlainText().isEmpty();

    // Показываем диалог подтверждения, если есть данные
    if (hasData) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Очистка полей",
                                      "Очистить все поля?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply != QMessageBox::Yes) {
            return;
        }
    }

    clearAllFields();
}

/**
 * @brief Очищает все поля и данные
 * 
 * Очищает текстовые поля исходного HTML и результата,
 * очищает объект currentTable и обновляет статусную строку.
 */
void MainWindow::clearAllFields() {
    ui->sourceTextEdit->clear();
    ui->resultTextEdit->clear();
    currentTable.clear();
    ui->statusbar->showMessage("Поля очищены");
}
