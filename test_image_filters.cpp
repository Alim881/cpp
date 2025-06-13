#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "image_filters.h"
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

// Helper function to load image with fallback paths
bool loadImage(Image& img, const std::string& filename = "input.png") {
    bool loaded = img.load(filename);
    if (!loaded && filename == "input.png") {
        loaded = img.load("../input.png");
    }
    return loaded;
}

// 1. Загрузка изображения
TEST_CASE("Image::load - положительный и отрицательный") {
    Image img;
    CHECK_FALSE(img.load("nonexistent.png"));  // Отриц: файл не существует
    CHECK(loadImage(img));                     // Положительный: загрузка существующего файла
}

// 2. Сохранение изображения
TEST_CASE("Image::save - положительный и отрицательный тесты") {
    Image img;
    REQUIRE(loadImage(img));                  // Загрузка изображения

    CHECK(img.save("output.png"));            // Положительный: сохранение работает
    CHECK_FALSE(img.save("/nonexistent_directory/output.png")); // Отрицательный: директория не существует
}

// 3. Получение размеров изображения
TEST_CASE("Image::getWidth/getHeight - положительный и отрицательный тесты") {
    Image img;
    CHECK(img.getWidth() == 0);   // Отрицательный: пустое изображение (до загрузки)
    CHECK(img.getHeight() == 0);

    REQUIRE(loadImage(img));       // Загрузка изображения
    CHECK(img.getWidth() > 0);    // Положительный: после загрузки размеры > 0
    CHECK(img.getHeight() > 0);
}

// 4. Получение пикселя
TEST_CASE("Image::getPixel - положительный и отрицательный тесты") {
    Image img;
    REQUIRE(loadImage(img));       // Загрузка изображения

    CHECK(img.getPixel(-1, 0) == std::vector<unsigned char>{0, 0, 0, 255});  // Отрицательный: координаты вне диапазона
    CHECK(img.getPixel(0, 0) != std::vector<unsigned char>{0, 0, 0, 0});     // Положительный: валидный пиксель
}

// 5. Установка пикселя
TEST_CASE("Image::setPixel - положительный и отрицательный тесты") {
    Image img;
    REQUIRE(loadImage(img));       // Загрузка изображения

    std::vector<unsigned char> color{255, 128, 0, 255};
    img.setPixel(0, 0, color);
    CHECK(img.getPixel(0, 0) == color); // Положительный: установка и получение пикселя

    img.setPixel(img.getWidth(), img.getHeight(), color); // Отрицательный: установка вне диапазона
    CHECK(img.getPixel(img.getWidth(), img.getHeight()) == std::vector<unsigned char>{0, 0, 0, 255});
}

// 6. Wave Distortion
TEST_CASE("applyWaveDistortion - положительный и отрицательный тесты") {
    Image img;
    REQUIRE(loadImage(img));       // Загрузка изображения

    // Сохраняем оригинальное изображение для сравнения
    Image original = img;

    // Положительный: с амплитудой 15 пикселей изображение меняется
    applyWaveDistortion(img, 15.0f);
    bool isChanged = false;
    for (int y = 0; y < img.getHeight(); ++y) {
        for (int x = 0; x < img.getWidth(); ++x) {
            if (img.getPixel(x, y) != original.getPixel(x, y)) {
                isChanged = true;
                break;
            }
        }
        if (isChanged) break;
    }
    CHECK(isChanged);

    // Восстанавливаем оригинальное изображение
    img = original;

    // Отрицательный: с амплитудой 0 изображение не меняется
    applyWaveDistortion(img, 0.0f);
    bool isUnchanged = true;
    for (int y = 0; y < img.getHeight(); ++y) {
        for (int x = 0; x < img.getWidth(); ++x) {
            if (img.getPixel(x, y) != original.getPixel(x, y)) {
                isUnchanged = false;
                break;
            }
        }
        if (!isUnchanged) break;
    }
    CHECK(isUnchanged);
}

// 7. Grayscale
TEST_CASE("applyGrayscale - положительный и отрицательный тесты") {
    Image img;
    REQUIRE(loadImage(img));       // Загрузка изображения

    // Сохраняем оригинал
    Image original = img;

    // Применяем фильтр grayscale
    applyGrayscale(img);

    // Положительный тест: после преобразования R == G == B
    bool isGrayscale = true;
    for (int y = 0; y < img.getHeight(); ++y) {
        for (int x = 0; x < img.getWidth(); ++x) {
            auto pixel = img.getPixel(x, y);
            if (!(pixel[0] == pixel[1] && pixel[1] == pixel[2])) {
                isGrayscale = false;
                break;
            }
        }
        if (!isGrayscale) break;
    }
    CHECK(isGrayscale);

    // Отрицательный тест: повторное применение не должно изменить результат
    Image beforeSecondApply = img;
    applyGrayscale(img);
    bool isUnchanged = true;
    for (int y = 0; y < img.getHeight(); ++y) {
        for (int x = 0; x < img.getWidth(); ++x) {
            if (img.getPixel(x, y) != beforeSecondApply.getPixel(x, y)) {
                isUnchanged = false;
                break;
            }
        }
        if (!isUnchanged) break;
    }
    CHECK(isUnchanged);
}

// 8. Glitch - Положительный тест: на стандартном изображении появляются артефакты
TEST_CASE("applyGlitch - стандартное изображение, появляются артефакты") {
    Image img;
    REQUIRE(loadImage(img));       // Загрузка input.png
    REQUIRE(img.getHeight() >= 10); // Убедимся, что изображение достаточно большое

    // Сохраняем оригинальное изображение для сравнения
    Image original = img;

    // Применяем фильтр glitch
    applyGlitch(img);

    // Проверяем, что изображение изменилось
    bool isChanged = false;
    for (int y = 0; y < img.getHeight(); ++y) {
        for (int x = 0; x < img.getWidth(); ++x) {
            if (img.getPixel(x, y) != original.getPixel(x, y)) {
                isChanged = true;
                break;
            }
        }
        if (isChanged) break;
    }
    CHECK(isChanged);
}


// 9. ColorNoise - Положительный и отрицательный тесты
TEST_CASE("applyColorNoise - положительный и отрицательный тесты") {
    Image img;
    REQUIRE(loadImage(img));       // Загрузка input.png

    // Сохраняем оригинальное изображение для сравнения
    Image original = img;

    // Положительный: с интенсивностью 0.5 изображение меняется
    applyColorNoise(img, 0.5f);
    bool isChanged = false;
    for (int y = 0; y < img.getHeight(); ++y) {
        for (int x = 0; x < img.getWidth(); ++x) {
            if (img.getPixel(x, y) != original.getPixel(x, y)) {
                isChanged = true;
                break;
            }
        }
        if (isChanged) break;
    }
    CHECK(isChanged);

    // Восстанавливаем оригинальное изображение
    img = original;

    // Отрицательный: с интенсивностью 0.0 изображение не меняется
    applyColorNoise(img, 0.0f);
    bool isUnchanged = true;
    for (int y = 0; y < img.getHeight(); ++y) {
        for (int x = 0; x < img.getWidth(); ++x) {
            if (img.getPixel(x, y) != original.getPixel(x, y)) {
                isUnchanged = false;
                break;
            }
        }
        if (!isUnchanged) break;
    }
    CHECK(isUnchanged);
}