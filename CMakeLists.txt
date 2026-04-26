cmake_minimum_required(VERSION 3.16)

project(HTMLTableCleaner VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===========================================================================
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
elseif(EXISTS "C:/Users/stedi/vcpkg/scripts/buildsystems/vcpkg.cmake")
    set(CMAKE_TOOLCHAIN_FILE "C:/Users/stedi/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# НАСТРОЙКА BOOST (Header-only версия)
# Указываем путь к Boost
set(BOOST_ROOT "C:/local/boost_1_90_0_b1" CACHE PATH "Путь к Boost")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}")

if(EXISTS "${BOOST_ROOT}/boost/regex.hpp")
    message(STATUS "Найден Boost в: ${BOOST_ROOT}")
    include_directories(${BOOST_ROOT})

    # Проверяем наличие необходимых заголовочных файлов
    if(EXISTS "${BOOST_ROOT}/boost/regex.hpp")
        message(STATUS "Boost.Regex найден (header-only)")
    else()
        message(WARNING "Boost.Regex не найден")
    endif()

    if(EXISTS "${BOOST_ROOT}/boost/spirit/include/qi.hpp")
        message(STATUS "Boost.Spirit найден (header-only)")
    else()
        message(WARNING "Boost.Spirit не найден")
    endif()
else()
    # Пробуем найти Boost в стандартных местах
    find_path(BOOST_INCLUDE_DIR boost/regex.hpp
        PATHS
        /usr/include
        /usr/local/include
        C:/local/boost_1_*
        "$ENV{ProgramFiles}/boost/boost_*"
        "$ENV{BOOST_ROOT}"
        "$ENV{BOOST_INCLUDEDIR}"
    )

    if(BOOST_INCLUDE_DIR)
        message(STATUS "Найден Boost в: ${BOOST_INCLUDE_DIR}")
        include_directories(${BOOST_INCLUDE_DIR})
    else()
        message(WARNING "Boost не найден. Используем std::regex вместо Boost.Regex")
        add_definitions(-DUSE_STD_REGEX)
    endif()
endif()
# ===========================================================================

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    Table.cpp
    Table.h
    Cell.h
    TableParser.cpp
    TableParser.h
    DatabaseManager.cpp
    DatabaseManager.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(HTMLTableCleaner
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        DatabaseManager.h
        DatabaseManager.cpp
    )
else()
    add_executable(HTMLTableCleaner
        ${PROJECT_SOURCES}
    )
endif()

# Подключаем только Qt (Boost header-only не требует линковки)
target_link_libraries(HTMLTableCleaner PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)

# Для отладки
if(WIN32)
    target_link_options(HTMLTableCleaner PRIVATE -Wl,--subsystem,console)
endif()

# Установка свойств
set_target_properties(HTMLTableCleaner PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(HTMLTableCleaner)
endif()

# Проверяем наличие заголовочных файлов Boost во время компиляции
add_custom_target(check_boost_depends ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Проверка зависимостей Boost..."
    COMMAND ${CMAKE_COMMAND} -E echo "Если будут ошибки компиляции, проверьте путь к Boost"
)

# Устанавливаем переменную для кода
add_definitions(-DBOOST_ALL_NO_LIB)  # Отключаем авто-линковку
