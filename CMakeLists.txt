
cmake_minimum_required(VERSION 3.10)
project(ImageFilters)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем модуль FetchContent
include(FetchContent)

# Настраиваем FetchContent для тишины и кэширования
set(FETCHCONTENT_QUIET OFF) # Можно включить для меньше вывода
set(FETCHCONTENT_UPDATES_DISCONNECTED ON) # Избегаем повторной загрузки, если кэш есть

# Объявляем загрузку zlib
FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG v1.3.1
)

# Объявляем загрузку libpng
FetchContent_Declare(
    libpng
    GIT_REPOSITORY https://github.com/glennrp/libpng.git
    GIT_TAG v1.6.43
)

# Делаем zlib и libpng доступными
FetchContent_MakeAvailable(zlib libpng)

# Указываем директории включаемых файлов
include_directories(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
include_directories(${libpng_SOURCE_DIR} ${libpng_BINARY_DIR})

# Основная библиотека с фильтрами
add_library(image_filters_lib STATIC image_filters.cpp)
add_dependencies(image_filters_lib png_static zlibstatic)

# Основное приложение
add_executable(image_filters main.cpp)
target_link_libraries(image_filters PRIVATE image_filters_lib png_static zlibstatic)

# Тестовый исполняемый файл
add_executable(tests test_image_filters.cpp)
target_link_libraries(tests PRIVATE image_filters_lib png_static zlibstatic)

# Включаем поддержку тестирования
enable_testing()

# Добавляем общий тест для ctest
add_test(NAME AllTests COMMAND tests)
