//#include <iostream>
//#include <fstream>
//#include <vector>
//
//// Функция для записи числа в файл в формате little-endian
//template<typename T>
//void writeNumber(std::ofstream& file, T value) {
//    char* ptr = reinterpret_cast<char*>(&value);
//    for (size_t i = 0; i < sizeof(T); ++i) {
//        file.write(ptr + i, 1);
//    }
//}
//
//int main() {
//    // Открываем файл для записи в бинарном режиме
//    std::ofstream file("test.ya3", std::ios::binary);
//    if (!file) {
//        std::cerr << "Не удалось открыть файл для записи." << std::endl;
//        return 1;
//    }
//
//    // Записываем ширину и высоту (2 байта каждое)
//    uint16_t width = 800;
//    uint16_t height = 600;
//    writeNumber(file, width);
//    writeNumber(file, height);
//
//    // Записываем количество бит на пиксель (1 байт)
//    uint8_t bitsPerPixel = 24;
//    writeNumber(file, bitsPerPixel);
//
//    // Записываем количество значений палитры (2 байта)
//    uint16_t paletteEntries = 2;
//    writeNumber(file, paletteEntries);
//
//    // Записываем записи палитры
//    std::vector<uint16_t> angles = { 45, 90 };
//    std::vector<uint16_t> lengths = { 100, 200 };
//    std::vector<uint8_t> rValues = { 255, 0 };
//    std::vector<uint8_t> gValues = { 0, 255 };
//    std::vector<uint8_t> bValues = { 0, 0 };
//    std::vector<uint8_t> aValues = { 255, 255 };
//
//    for (size_t i = 0; i < paletteEntries; ++i) {
//        writeNumber(file, angles[i]);
//        writeNumber(file, lengths[i]);
//        writeNumber(file, rValues[i]);
//        writeNumber(file, gValues[i]);
//        writeNumber(file, bValues[i]);
//        writeNumber(file, aValues[i]);
//    }
//
//    // Записываем пиксели (вещественные числа X и Y, 2 байта каждое)
//    std::vector<uint16_t> xValues = { 100, 200, 300 };
//    std::vector<uint16_t> yValues = { 150, 250, 350 };
//
//    for (size_t i = 0; i < xValues.size(); ++i) {
//        writeNumber(file, xValues[i]);
//        writeNumber(file, yValues[i]);
//    }
//
//    // Закрываем файл
//    file.close();
//
//    std::cout << "DONE! test.ya3 SUCCESS." << std::endl;
//    return 0;
//}



/* V3 ====== V3 ======= V3 ====== V3 ======= V3 ====== V3 ======= V3 ====== V3 ======= V3*/


#include <iostream>
#include <fstream>
#include <vector>

// Функция для записи числа в файл в формате little-endian
template<typename T>
void writeNumber(std::ofstream& file, T value) {
    char* ptr = reinterpret_cast<char*>(&value);
    for (size_t i = 0; i < sizeof(T); ++i) {
        file.write(ptr + i, 1);
    }
}

int main() {
    // Открываем файл для записи в бинарном режиме
    std::ofstream file("E:/Отчеты/Гуляев/PKG_Lab2/MainApp/test.ya3", std::ios::binary);
    if (!file) {
        std::cerr << "Не удалось открыть файл для записи." << std::endl;
        return 1;
    }

    // Записываем ширину и высоту (2 байта каждое)
    uint16_t width = 1000;
    uint16_t height = 1000;
    writeNumber(file, width);
    writeNumber(file, height);

    // Записываем количество бит на пиксель (1 байт)
    uint8_t bitsPerPixel = 24;
    writeNumber(file, bitsPerPixel);

    // Записываем количество значений палитры (2 байта)
    uint16_t paletteEntries = 3;
    writeNumber(file, paletteEntries);

    // Записываем записи палитры
    std::vector<uint16_t> angles = { 120, 120, 120 };
    std::vector<uint16_t> lengths = { 15, 15, 15};
    std::vector<uint8_t> rValues = { 255, 0, 0};
    std::vector<uint8_t> gValues = { 0, 255, 0};
    std::vector<uint8_t> bValues = { 0, 0, 255};
    std::vector<uint8_t> aValues = { 255, 255, 255};

    for (size_t i = 0; i < paletteEntries; ++i) {
        writeNumber(file, angles[i]);
        writeNumber(file, lengths[i]);
        writeNumber(file, rValues[i]);
        writeNumber(file, gValues[i]);
        writeNumber(file, bValues[i]);
        writeNumber(file, aValues[i]);
    }

    // Записываем пиксели (вещественные числа X и Y, 2 байта каждое)
    std::vector<float> xValues = { 35.5f, -140.75f, 25.0f, 100.0f, -200.0f, -59.5f};
    std::vector<float> yValues = { 40.3f, 100.6f, -350.9f, -50.0f, 150.0f, 300.0f};

    for (size_t i = 0; i < xValues.size(); ++i) {
        writeNumber(file, xValues[i]);
        writeNumber(file, yValues[i]);
    }

    // Закрываем файл
    file.close();

    std::cout << "Файл test.ya3 успешно создан." << std::endl;
    return 0;
}


/* V4 ====== V4 ======= V4 ====== V4 ======= V4 ====== V4 ======= V4 ====== V4 ======= V4*/

