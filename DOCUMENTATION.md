# HTML Table Cleaner — актуальная документация

## Что это за проект

`HTMLTableCleaner` — настольное приложение на `Qt Widgets` (C++17), которое:

- принимает HTML (в том числе из Microsoft Word);
- находит и очищает **все таблицы** в документе;
- сохраняет структуру merged ячеек (`rowspan` / `colspan`);
- показывает результат как текст (чистый HTML) и как визуальную таблицу;
- умеет экспортировать очищенные данные в `SQLite`.

---

## Функциональность приложения

- **Открытие HTML-файла** (`.html`, `.htm`) через диалог.
- **Очистка таблиц**:
  - нормализация HTML;
  - удаление Word-специфичных атрибутов и мусора;
  - очистка содержимого ячеек;
  - поддержка нескольких таблиц в одном документе.
- **Просмотр результата**:
  - текстовый HTML в `resultTextEdit`;
  - табличная визуализация в `resultTableWidget`.
- **Сохранение результата** в HTML-файл (UTF-8).
- **Копирование результата** в буфер обмена.
- **Экспорт таблиц в SQLite** (каждая таблица экспортируется в отдельную таблицу БД).
- **Горячие клавиши**:
  - `Ctrl+O` — открыть;
  - `Ctrl+R` — очистить;
  - `Ctrl+S` — сохранить;
  - `Ctrl+C` — копировать;
  - `Ctrl+L` / `Esc` — очистить поля;
  - `Ctrl+E` — экспорт в БД.

---

## Архитектура и ключевые модули

### `main.cpp`

Точка входа: создаёт `QApplication`, `MainWindow`, запускает `exec()`.

### `mainwindow.h` / `mainwindow.cpp`

Главная UI-логика:

- обработчики кнопок и горячих клавиш;
- запуск пайплайна очистки (`TableParser`);
- хранение текущего результата в `std::vector<Table> currentTables`;
- визуализация нескольких таблиц в `QTableWidget`;
- экспорт в БД через `DatabaseManager`.

Ключевые методы:

- `onOpenButtonClicked()`
- `onCleanButtonClicked()`
- `updateResultDisplay()`
- `visualizeTables(const std::vector<Table>&)`
- `onSaveButtonClicked()`
- `onCopyButtonClicked()`
- `onClearButtonClicked()`
- `onExportToDbClicked()`

### `TableParser.h` / `TableParser.cpp`

Статический парсер/очиститель HTML.

Публичный API:

- `parseSimpleHtml(const std::string&) -> Table`
- `parseMultipleTables(const std::string&) -> std::vector<Table>`
- `extractTableFromWordHtml(const std::string&) -> std::string`
- `cleanWordHtml(const std::string&) -> std::string`

Что делает:

- удаляет комментарии, CDATA, условные Word-блоки;
- оставляет только набор допустимых тегов для таблиц;
- удаляет `class`, `style`, namespace-атрибуты Word;
- очищает контент ячеек (форматирующие теги, сущности, лишние пробелы);
- фильтрует фиктивные технические Word-ячейки;
- корректно строит сетку merged ячеек.

### `Cell.h`

Полиморфная модель ячеек:

- `AbstractCell` — общий интерфейс;
- `RealCell` — реальная ячейка с контентом и span;
- `ReferenceCell` — ссылка на целевую ячейку для покрытия merged-областей.

Тип алиаса:

- `using CellPtr = std::shared_ptr<AbstractCell>;`

### `Table.h` / `Table.cpp`

Контейнер табличной сетки:

- `addRow(const std::vector<CellPtr>&)`
- `getCell(row, col)`
- `getCellContent(row, col)`
- `getRowCount()`, `getMaxColumnCount()`
- `toHtml()` — сериализация в чистый HTML.

Особенность `toHtml()`:

- `ReferenceCell` не выводятся отдельно, чтобы не дублировать merged-контент.

### `DatabaseManager.h` / `DatabaseManager.cpp`

Работа с `QtSql` и `QSQLITE`:

- `connect(dbFileName)` — подключение/создание БД;
- `createTable(tableName, columnCount)` — создание целевой таблицы;
- `importTable(table, tableName)` — запись данных таблицы;
- `close()`.

Экспорт учитывает `ReferenceCell`: при записи берётся контент целевой (`target`) ячейки.

### `mainwindow.ui`

Состав UI:

- `sourceTextEdit` — исходный HTML;
- `resultTextEdit` — очищенный HTML;
- `resultTableWidget` — визуализация таблиц;
- кнопки: открыть, очистить, сохранить, копировать, очистить поля, экспорт в БД;
- `statusbar`.

### `CMakeLists.txt`

Сборка через CMake:

- `CMake >= 3.16`
- `C++17`
- `Qt Widgets + Qt Sql` (Qt5/Qt6)
- подключены `AUTOUIC`, `AUTOMOC`, `AUTORCC`
- в проект включены `DatabaseManager.*` и весь основной код.

Примечание: в конфигурации есть блоки проверки `Boost`, но текущая основная логика очистки таблиц работает на стандартной библиотеке и Qt.

---

## Поток обработки данных

1. Пользователь загружает HTML или вставляет его вручную.
2. `onCleanButtonClicked()` вызывает `TableParser::cleanWordHtml()`.
3. Полученный очищенный HTML дополнительно разбирается в `parseMultipleTables()`.
4. Результат сохраняется в `currentTables`.
5. UI обновляется:
   - текстовый HTML (`updateResultDisplay()`);
   - визуальная таблица (`visualizeTables()`).
6. Дальше пользователь может:
   - сохранить HTML;
   - скопировать HTML;
   - экспортировать таблицы в SQLite.

---

## Ограничения текущей реализации

- Парсер основан на строковой обработке HTML (без полноценного DOM-парсера).
- Поддержка нацелена в первую очередь на таблицы из Word и схожих источников.
- При сильно невалидном HTML возможны частичные потери структуры.

---

## Короткая памятка для разработчика

- Основная бизнес-логика сосредоточена в `TableParser`.
- UI-оркестрация и взаимодействие с пользователем — в `MainWindow`.
- Модель merged ячеек реализована через `RealCell` + `ReferenceCell`.
- Любые изменения в структуре `Table` должны проверяться в:
  - `toHtml()`,
  - `visualizeTables()`,
  - `DatabaseManager::importTable()`.
