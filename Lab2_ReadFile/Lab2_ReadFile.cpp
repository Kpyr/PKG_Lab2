//#include <iostream>
//#include <fstream>
//#include <vector>
//
//// Функция для чтения числа из файла в формате little-endian
//template<typename T>
//T readNumber(std::ifstream& file) {
//    T value;
//    char* ptr = reinterpret_cast<char*>(&value);
//    for (size_t i = 0; i < sizeof(T); ++i) {
//        file.read(ptr + i, 1);
//    }
//    return value;
//}
//
//int main() {
//    // Открываем файл для чтения в бинарном режиме
//    std::ifstream file("test.ya3", std::ios::binary);
//    if (!file) {
//        std::cerr << "Cant read File." << std::endl;
//        return 1;
//    }
//
//    // Читаем ширину и высоту (2 байта каждое)
//    uint16_t width = readNumber<uint16_t>(file);
//    uint16_t height = readNumber<uint16_t>(file);
//    std::cout << "W: " << width << std::endl;
//    std::cout << "H: " << height << std::endl;
//
//    // Читаем количество бит на пиксель (1 байт)
//    uint8_t bitsPerPixel = readNumber<uint8_t>(file);
//    std::cout << "K: " << static_cast<int>(bitsPerPixel) << std::endl;
//
//    // Читаем количество значений палитры (2 байта)
//    uint16_t paletteEntries = readNumber<uint16_t>(file);
//    std::cout << "N: " << paletteEntries << std::endl;
//
//    // Читаем записи палитры
//    std::vector<uint16_t> angles(paletteEntries);
//    std::vector<uint16_t> lengths(paletteEntries);
//    std::vector<uint8_t> rValues(paletteEntries);
//    std::vector<uint8_t> gValues(paletteEntries);
//    std::vector<uint8_t> bValues(paletteEntries);
//    std::vector<uint8_t> aValues(paletteEntries);
//
//    for (size_t i = 0; i < paletteEntries; ++i) {
//        angles[i] = readNumber<uint16_t>(file);
//        lengths[i] = readNumber<uint16_t>(file);
//        rValues[i] = readNumber<uint8_t>(file);
//        gValues[i] = readNumber<uint8_t>(file);
//        bValues[i] = readNumber<uint8_t>(file);
//        aValues[i] = readNumber<uint8_t>(file);
//    }
//
//    // Выводим записи палитры
//    for (size_t i = 0; i < paletteEntries; ++i) {
//        std::cout << "id: " << i + 1 << ":" << std::endl;
//        std::cout << "  Angle: " << angles[i] << std::endl;
//        std::cout << "  Len: " << lengths[i] << std::endl;
//        std::cout << "  R: " << static_cast<int>(rValues[i]) << std::endl;
//        std::cout << "  G: " << static_cast<int>(gValues[i]) << std::endl;
//        std::cout << "  B: " << static_cast<int>(bValues[i]) << std::endl;
//        std::cout << "  A: " << static_cast<int>(aValues[i]) << std::endl;
//    }
//
//    // Читаем пиксели (вещественные числа X и Y, 2 байта каждое)
//    std::vector<uint16_t> xValues;
//    std::vector<uint16_t> yValues;
//
//    while (file) {
//        uint16_t x = readNumber<uint16_t>(file);
//        uint16_t y = readNumber<uint16_t>(file);
//        if (file) {
//            xValues.push_back(x);
//            yValues.push_back(y);
//        }
//    }
//
//    // Выводим пиксели
//    for (size_t i = 0; i < xValues.size(); ++i) {
//        std::cout << "Pixel " << i + 1 << ":" << std::endl;
//        std::cout << "  X: " << xValues[i] << std::endl;
//        std::cout << "  Y: " << yValues[i] << std::endl;
//    }
//
//    // Закрываем файл
//    file.close();
//
//    std::cout << "DONE! test.ya3." << std::endl;
//    return 0;
//}

#include <iostream>
#include <fstream>
#include <vector>

// Функция для чтения числа из файла в формате little-endian
template<typename T>
T readNumber(std::ifstream& file) {
    T value;
    char* ptr = reinterpret_cast<char*>(&value);
    for (size_t i = 0; i < sizeof(T); ++i) {
        file.read(ptr + i, 1);
    }
    return value;
}

int main() {
    // Открываем файл для чтения в бинарном режиме
    std::ifstream file("test.ya3", std::ios::binary);
    if (!file) {
        std::cerr << "ERROR." << std::endl;
        return 1;
    }

    // Читаем ширину и высоту (2 байта каждое)
    uint16_t width = readNumber<uint16_t>(file);
    uint16_t height = readNumber<uint16_t>(file);
    std::cout << "W: " << width << std::endl;
    std::cout << "H: " << height << std::endl;

    // Читаем количество бит на пиксель (1 байт)
    uint8_t bitsPerPixel = readNumber<uint8_t>(file);
    std::cout << "K: " << static_cast<int>(bitsPerPixel) << std::endl;

    // Читаем количество значений палитры (2 байта)
    uint16_t paletteEntries = readNumber<uint16_t>(file);
    std::cout << "N: " << paletteEntries << std::endl;

    // Читаем записи палитры
    std::vector<uint16_t> angles(paletteEntries);
    std::vector<uint16_t> lengths(paletteEntries);
    std::vector<uint8_t> rValues(paletteEntries);
    std::vector<uint8_t> gValues(paletteEntries);
    std::vector<uint8_t> bValues(paletteEntries);
    std::vector<uint8_t> aValues(paletteEntries);

    for (size_t i = 0; i < paletteEntries; ++i) {
        angles[i] = readNumber<uint16_t>(file);
        lengths[i] = readNumber<uint16_t>(file);
        rValues[i] = readNumber<uint8_t>(file);
        gValues[i] = readNumber<uint8_t>(file);
        bValues[i] = readNumber<uint8_t>(file);
        aValues[i] = readNumber<uint8_t>(file);
    }

    // Выводим записи палитры
    for (size_t i = 0; i < paletteEntries; ++i) {
        std::cout << "Pal id: " << i + 1 << ":" << std::endl;
        std::cout << "  Angel: " << angles[i] << std::endl;
        std::cout << "  LEn: " << lengths[i] << std::endl;
        std::cout << "  R: " << static_cast<int>(rValues[i]) << std::endl;
        std::cout << "  G: " << static_cast<int>(gValues[i]) << std::endl;
        std::cout << "  B: " << static_cast<int>(bValues[i]) << std::endl;
        std::cout << "  A: " << static_cast<int>(aValues[i]) << std::endl;
    }

    // Читаем пиксели (вещественные числа X и Y, 2 байта каждое)
    std::vector<float> xValues;
    std::vector<float> yValues;

    while (file) {
        float x = readNumber<float>(file);
        float y = readNumber<float>(file);
        if (file) {
            xValues.push_back(x);
            yValues.push_back(y);
        }
    }

    // Выводим пиксели
    for (size_t i = 0; i < xValues.size(); ++i) {
        std::cout << "Pixel id: " << i + 1 << ":" << std::endl;
        std::cout << "  X: " << xValues[i] << std::endl;
        std::cout << "  Y: " << yValues[i] << std::endl;
    }

    // Закрываем файл
    file.close();

    std::cout << "DONE! test.ya3." << std::endl;
    return 0;
}