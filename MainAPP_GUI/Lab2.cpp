#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>




// Функция для записи числа
template<typename T>
void writeNumber(std::ofstream& file, T value) {
    char* ptr = reinterpret_cast<char*>(&value);
    for (size_t i = 0; i < sizeof(T); ++i) {
        file.write(ptr + i, 1);
    }
}

// Функция Сравнения
template <typename T>
T clamp(T value, T min, T max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// Функция для чтения числа из файла
template<typename T>
T readNumber(std::ifstream& file) {
    T value;
    char* ptr = reinterpret_cast<char*>(&value);
    for (size_t i = 0; i < sizeof(T); ++i) {
        file.read(ptr + i, 1);
    }
    return value;
}

// Структура палитры
struct PaletteEntry {
    int angle;
    int length;
    int r, g, b, a;
};

// Функция для загрузки пикселей из файла
std::vector<sf::RectangleShape> loadFile(const std::string& filename, uint16_t& width, uint16_t& height) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<sf::RectangleShape> pixelShapes;

    bool isCorrect = false;

    if (!file) {
        std::cerr << "Не удалось открыть файл." << std::endl;
        return pixelShapes;
    }
    if (filename[0] == 's') {
        isCorrect = true;
    }

    // Чтение параметров изображения
    width = readNumber<uint16_t>(file);
    height = readNumber<uint16_t>(file);
    uint8_t bitsPerPixel = readNumber<uint8_t>(file);
    uint16_t paletteEntries = readNumber<uint16_t>(file);

    std::vector<sf::Vector2f> recoloredPixels;
    std::vector<std::vector<int>> newColors;

    std::vector<uint16_t> angles(paletteEntries);
    std::vector<uint8_t> rValues(paletteEntries);
    std::vector<uint8_t> gValues(paletteEntries);
    std::vector<uint8_t> bValues(paletteEntries);
    std::vector<uint8_t> aValues(paletteEntries);

    int id = 0;

    // Чтение палитры
    std::vector<PaletteEntry> palette(paletteEntries);
    for (auto& entry : palette) {
        entry.angle = readNumber<uint16_t>(file);
        entry.length = readNumber<uint16_t>(file);
        entry.r = readNumber<uint8_t>(file);
        entry.g = readNumber<uint8_t>(file);
        entry.b = readNumber<uint8_t>(file);
        entry.a = readNumber<uint8_t>(file);

        angles[id] = entry.angle;
        rValues[id] = entry.r;
        gValues[id] = entry.g;
        bValues[id] = entry.b;
        aValues[id] = entry.a;
        id++;
    }

    // Чтение координат пикселей
    std::vector<sf::Vector2f> pixels;
    std::vector<int> scales;
    std::vector<int> newScales;
    int tmpScale = 1;


    float x = readNumber<float>(file);
    float y = readNumber<float>(file);
    int scale = readNumber<uint16_t>(file);
    if (file) {
        pixels.emplace_back(x, y);
        scales.emplace_back(scale);
        tmpScale = scales[0];
    }

    tmpScale = scale;


    for (int i = 0; i < ((width/tmpScale) * (height/tmpScale)) - 1; i++) {
        float x = readNumber<float>(file);
        float y = readNumber<float>(file);
        int scale = readNumber<uint16_t>(file);
        if (file) {
            pixels.emplace_back(x, y);
            scales.emplace_back(scale);
            tmpScale = scales[0];
        }
    }

    while (file) {
        float x = readNumber<float>(file);
        float y = readNumber<float>(file);
        int scale = readNumber<uint16_t>(file);

        int r = readNumber<uint8_t>(file);
        int g = readNumber<uint8_t>(file);
        int b = readNumber<uint8_t>(file);
        int a = readNumber<uint8_t>(file);

        if (file) {
            recoloredPixels.emplace_back(x, y);
            newScales.emplace_back(scale);
            newColors.push_back({ r, g, b, a });
        }
    }
    file.close();

    /*while (file) {
        float x = readNumber<float>(file);
        float y = readNumber<float>(file);
        int scale = readNumber<uint16_t>(file);
        if (file) {
            pixels.emplace_back(x, y);
            scales.emplace_back(scale);
        }
    }*/
    //file.close();

    tmpScale = scales[0];

    float tempWidth = 0;
    float tempHeight = 0;

    int tmpID = 0;

    std::vector<std::vector<float>> allPixelsPos;
    std::vector<std::vector<int>> idPixelsPos;

    // Создание фигур для пикселей
    sf::Vector2f center(std::min(1920, 1080) / 2.0f, std::min(1920, 1080) / 2.0f);
    int _id1 = 0;
    int _id2 = 0;

    int id1 = width / tmpScale;
    int id2 = height / tmpScale;

    for (const auto& pixel : pixels) {
        sf::Vector2f pos = pixel + center;
        sf::Vector2f pixelPos = sf::Vector2f(center.x + tempWidth + tmpScale, center.y + tempHeight + tmpScale);
        allPixelsPos.push_back({ pixelPos.x, pixelPos.y });
        idPixelsPos.push_back({_id1,_id2});

        _id1++;
        if (_id1 == id1) {
            _id1 = 0;
            _id2++;
        }

        sf::Vector2f delta = pos - center;

        float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        float angle = std::atan2(delta.y, delta.x);

        if (angle < 0) {
            angle += 2 * 3.14159265f;  // Преобразуем отрицательный угол в положительный
        }

        int sectorIndex = -1;
        float totalAngle = 0.0f;
        for (size_t j = 0; j < paletteEntries; ++j) {
            float sectorAngle = angles[j] * 3.14159265f / 180.0f;

            if (angle >= totalAngle && angle < totalAngle + sectorAngle + 1e-5f) {
                sectorIndex = j;
                break;
            }
            totalAngle += sectorAngle;
        }

        float radius = std::min(width, height) / 2.0f;

        if (sectorIndex != -1) {
            sf::RectangleShape shape(sf::Vector2f(1.0f * tmpScale, 1.0f * tmpScale));
            shape.setPosition(pixelPos);
            sf::Color color(rValues[sectorIndex], gValues[sectorIndex], bValues[sectorIndex], aValues[sectorIndex]);
            shape.setFillColor(color);

            // Добавляем обводку
            shape.setOutlineThickness(1.0f);  // Толщина обводки
            shape.setOutlineColor(sf::Color(192, 192, 192));  // Светло-серый цвет

            pixelShapes.push_back(shape);
        }

        tempWidth += tmpScale;
        tmpID += tmpScale;
        if (tmpID >= width) {
            tempWidth = 0;
            tmpID = 0;
            tempHeight += tmpScale;
        }
    }

    
    for (size_t i = 0; i < recoloredPixels.size(); ++i) {
        
        
        int id = 0;

        sf::Vector2f pixelPos;
        pixelPos.x = recoloredPixels[i].x;
        pixelPos.y = recoloredPixels[i].y;

        int tm1 = 0, tm2 = 0;
        bool fl = true;
        while (fl)
        {
            if (tm1 == (int)pixelPos.x && tm2 == (int)pixelPos.y) {
                fl = false;
            }


            /*if (tm1 == id1) {
                tm1 = 0;
                tm2 ++;
            }*/

            if (fl){

                if (tm1 > id1 || tm2 > id2) {
                    break;
                }
                tm1++;
                if (tm1 == id1) {
                    tm1 = 0;
                    tm2++;
                }
                id++;
            }
        }

        sf::Color color(newColors[i][0], newColors[i][1], newColors[i][2], newColors[i][3]);
        pixelShapes[id].setFillColor(color);
    
    }

    return pixelShapes;
}
// Функция для изменения контрастности
void adjustContrast(const std::string& inputFilename, const std::string& outputFilename, float contrastFactor) {
    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Не удалось открыть исходный файл." << std::endl;
        return;
    }

    // Чтение параметров изображения
    uint16_t width = readNumber<uint16_t>(inputFile);
    uint16_t height = readNumber<uint16_t>(inputFile);
    uint8_t bitsPerPixel = readNumber<uint8_t>(inputFile);
    uint16_t paletteEntries = readNumber<uint16_t>(inputFile);

    std::vector<sf::Vector2f> recoloredPixels;
    std::vector<std::vector<int>> newColors;

    std::vector<uint16_t> angles(paletteEntries);
    std::vector<uint8_t> rValues(paletteEntries);
    std::vector<uint8_t> gValues(paletteEntries);
    std::vector<uint8_t> bValues(paletteEntries);
    std::vector<uint8_t> aValues(paletteEntries);

    int id = 0;

    // Чтение палитры
    std::vector<PaletteEntry> palette(paletteEntries);
    for (auto& entry : palette) {
        entry.angle = readNumber<uint16_t>(inputFile);
        entry.length = readNumber<uint16_t>(inputFile);
        entry.r = readNumber<uint8_t>(inputFile);
        entry.g = readNumber<uint8_t>(inputFile);
        entry.b = readNumber<uint8_t>(inputFile);
        entry.a = readNumber<uint8_t>(inputFile);

        angles[id] = entry.angle;
        rValues[id] = entry.r;
        gValues[id] = entry.g;
        bValues[id] = entry.b;
        aValues[id] = entry.a;
        id++;
    }

    // Чтение координат пикселей
    std::vector<sf::Vector2f> pixels;
    std::vector<int> scales;
    std::vector<int> newScales;
    int tmpScale = 1;


    float x = readNumber<float>(inputFile);
    float y = readNumber<float>(inputFile);
    int scale = readNumber<uint16_t>(inputFile);
    if (inputFile) {
        pixels.emplace_back(x, y);
        scales.emplace_back(scale);
        tmpScale = scales[0];
    }

    tmpScale = scale;


    for (int i = 0; i < ((width / tmpScale) * (height / tmpScale)) - 1; i++) {
        float x = readNumber<float>(inputFile);
        float y = readNumber<float>(inputFile);
        int scale = readNumber<uint16_t>(inputFile);
        if (inputFile) {
            pixels.emplace_back(x, y);
            scales.emplace_back(scale);
            tmpScale = scales[0];
        }
    }

    while (inputFile) {
        float x = readNumber<float>(inputFile);
        float y = readNumber<float>(inputFile);
        int scale = readNumber<uint16_t>(inputFile);

        int r = readNumber<uint8_t>(inputFile);
        int g = readNumber<uint8_t>(inputFile);
        int b = readNumber<uint8_t>(inputFile);
        int a = readNumber<uint8_t>(inputFile);

        if (inputFile) {
            recoloredPixels.emplace_back(x, y);
            newScales.emplace_back(scale);
            newColors.push_back({ r, g, b, a });
        }
    }
    inputFile.close();

    // Применение контрастности к палитре
    for (auto& entry : palette) {
        entry.r = clamp<int>((((entry.r - 128) * contrastFactor) + 128), 0, 255);
        entry.g = clamp<int>((((entry.g - 128) * contrastFactor) + 128), 0, 255);
        entry.b = clamp<int>((((entry.b - 128) * contrastFactor) + 128), 0, 255);
        // 
    }


    for (size_t i = 0; i < newColors.size(); ++i) {
        newColors[i][0] = clamp<int>((((newColors[i][0] - 128) * contrastFactor) + 128), 0, 255);
        newColors[i][1] = clamp<int>((((newColors[i][1] - 128) * contrastFactor) + 128), 0, 255);
        newColors[i][2] = clamp<int>((((newColors[i][2] - 128) * contrastFactor) + 128), 0, 255);
        newColors[i][3] = clamp<int>((((newColors[i][3] - 128) * contrastFactor) + 128), 0, 255);
    }

    // Запись в новый файл
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Не удалось создать выходной файл." << std::endl;
        return;
    }



    writeNumber(outputFile, width);
    writeNumber(outputFile, height);
    writeNumber(outputFile, bitsPerPixel);
    writeNumber(outputFile, paletteEntries);

    for (const auto& entry : palette) {
        writeNumber(outputFile, static_cast<uint16_t>(entry.angle));
        writeNumber(outputFile, static_cast<uint16_t>(entry.length));
        writeNumber(outputFile, static_cast<uint8_t>(entry.r));
        writeNumber(outputFile, static_cast<uint8_t>(entry.g));
        writeNumber(outputFile, static_cast<uint8_t>(entry.b));
        writeNumber(outputFile, static_cast<uint8_t>(entry.a));
    }

    for (size_t i = 0; i < pixels.size(); ++i) {
        writeNumber(outputFile, pixels[i].x);
        writeNumber(outputFile, pixels[i].y);
        writeNumber(outputFile, static_cast<uint16_t>(scales[i]));
    }

    for (size_t i = 0; i < recoloredPixels.size(); ++i) {
        writeNumber(outputFile, recoloredPixels[i].x);
        writeNumber(outputFile, recoloredPixels[i].y);
        writeNumber(outputFile, static_cast<uint16_t>(newScales[i]));

        writeNumber(outputFile, static_cast<uint8_t>(newColors[i][0]));
        writeNumber(outputFile, static_cast<uint8_t>(newColors[i][1]));
        writeNumber(outputFile, static_cast<uint8_t>(newColors[i][2]));
        writeNumber(outputFile, static_cast<uint8_t>(newColors[i][3]));
    }

    outputFile.close();
}
// Функция для увеличения размеров изображения
void scaleImage(const std::string& inputFilename, const std::string& outputFilename, int scaleFactor) {
    // Открытие исходного файла
    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Не удалось открыть исходный файл." << std::endl;
        return;
    }

    // Чтение параметров изображения
    uint16_t width = readNumber<uint16_t>(inputFile);
    uint16_t height = readNumber<uint16_t>(inputFile);
    uint8_t bitsPerPixel = readNumber<uint8_t>(inputFile);
    uint16_t paletteEntries = readNumber<uint16_t>(inputFile);

    std::vector<PaletteEntry> palette(paletteEntries);
    for (auto& entry : palette) {
        entry.angle = readNumber<uint16_t>(inputFile);
        entry.length = readNumber<uint16_t>(inputFile);
        entry.r = readNumber<uint8_t>(inputFile);
        entry.g = readNumber<uint8_t>(inputFile);
        entry.b = readNumber<uint8_t>(inputFile);
        entry.a = readNumber<uint8_t>(inputFile);
    }

    uint16_t scale;
    // Чтение пикселей
    std::vector<sf::Vector2f> pixels;
    std::vector<int> newScales;
    std::vector<sf::Vector2f> recoloredPixels;
    std::vector<std::vector<int>> newColors;

    float x = readNumber<float>(inputFile);
    float y = readNumber<float>(inputFile);
    uint16_t tmpScale = readNumber<uint16_t>(inputFile);
    if (inputFile) {
        pixels.emplace_back(x, y);
        scale = tmpScale;
    }

    for (size_t i = 0; i < ((width / scale) * (height / scale)) - 1; ++i) {
        float x = readNumber<float>(inputFile);
        float y = readNumber<float>(inputFile);
        uint16_t tmpScale = readNumber<uint16_t>(inputFile);
        if (inputFile) {
            pixels.emplace_back(x, y);
            scale = tmpScale;
        }
    }
    
    while (inputFile) {
        float x = readNumber<float>(inputFile);
        float y = readNumber<float>(inputFile);
        int scale = readNumber<uint16_t>(inputFile);

        int r = readNumber<uint8_t>(inputFile);
        int g = readNumber<uint8_t>(inputFile);
        int b = readNumber<uint8_t>(inputFile);
        int a = readNumber<uint8_t>(inputFile);

        if (inputFile) {
            recoloredPixels.emplace_back(x, y);
            newScales.emplace_back(scale);
            newColors.push_back({ r, g, b, a });
        }
    }
    inputFile.close();

    scaleFactor = scaleFactor;

    // Изменение размеров изображения
    uint16_t newWidth = width * scaleFactor;
    uint16_t newHeight = height * scaleFactor;
    //uint16_t newWidth = width;
    //uint16_t newHeight = height;
    std::vector<sf::Vector2f> scaledPixels;

    for (const auto& pixel : pixels) {
        float newX = pixel.x * static_cast<float>(scaleFactor);
        float newY = pixel.y * static_cast<float>(scaleFactor);
        scaledPixels.emplace_back(newX, newY);
    }

    // Запись в новый файл
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Не удалось создать выходной файл." << std::endl;
        return;
    }

    writeNumber(outputFile, newWidth);
    writeNumber(outputFile, newHeight);
    writeNumber(outputFile, bitsPerPixel);
    writeNumber(outputFile, paletteEntries);

    for (const auto& entry : palette) {
        writeNumber(outputFile, static_cast<uint16_t>(entry.angle));
        writeNumber(outputFile, static_cast<uint16_t>(entry.length));
        writeNumber(outputFile, static_cast<uint8_t>(entry.r));
        writeNumber(outputFile, static_cast<uint8_t>(entry.g));
        writeNumber(outputFile, static_cast<uint8_t>(entry.b));
        writeNumber(outputFile, static_cast<uint8_t>(entry.a));
    }

    for (const auto& pixel : scaledPixels) {
        writeNumber(outputFile, pixel.x);
        writeNumber(outputFile, pixel.y);
        writeNumber(outputFile, (uint16_t)(scaleFactor * (int)scale));
    }

	for (size_t i = 0; i < recoloredPixels.size(); ++i) {
		writeNumber(outputFile, recoloredPixels[i].x);
		writeNumber(outputFile, recoloredPixels[i].y);
		writeNumber(outputFile, static_cast<uint16_t>(newScales[i] * scaleFactor));

		writeNumber(outputFile, static_cast<uint8_t>(newColors[i][0]));
		writeNumber(outputFile, static_cast<uint8_t>(newColors[i][1]));
		writeNumber(outputFile, static_cast<uint8_t>(newColors[i][2]));
		writeNumber(outputFile, static_cast<uint8_t>(newColors[i][3]));
	}

    outputFile.close();
}
// Функция для изменения цвета пикселя в указанных координатах
void changePixelColor(const std::string& filename, float _x, float _y, int _r, int _g, int _b, int _a) {
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Не удалось открыть исходный файл." << std::endl;
        return;
    }

    // Чтение параметров изображения
    uint16_t width = readNumber<uint16_t>(inputFile);
    uint16_t height = readNumber<uint16_t>(inputFile);
    uint8_t bitsPerPixel = readNumber<uint8_t>(inputFile);
    uint16_t paletteEntries = readNumber<uint16_t>(inputFile);

    std::vector<PaletteEntry> palette(paletteEntries);
    for (auto& entry : palette) {
        entry.angle = readNumber<uint16_t>(inputFile);
        entry.length = readNumber<uint16_t>(inputFile);
        entry.r = readNumber<uint8_t>(inputFile);
        entry.g = readNumber<uint8_t>(inputFile);
        entry.b = readNumber<uint8_t>(inputFile);
        entry.a = readNumber<uint8_t>(inputFile);
    }

    uint16_t scale;

    // Чтение пикселей
    std::vector<sf::Vector2f> pixels;
    std::vector<int> scales;
    std::vector<int> newScales;


    std::vector<sf::Vector2f> recoloredPixels;
    std::vector<std::vector<int>> newColors;

    int tmpScale;

    float x = readNumber<float>(inputFile);
    float y = readNumber<float>(inputFile);
    scale = readNumber<uint16_t>(inputFile);
    if (inputFile) {
        pixels.emplace_back(x, y);
        scales.emplace_back(scale);
        tmpScale = scales[0];
    }

    for (int i = 0; i < ((width/ tmpScale) * (height/ tmpScale)) - 1; i++) {
        float x = readNumber<float>(inputFile);
        float y = readNumber<float>(inputFile);
        uint16_t tmpScale = readNumber<uint16_t>(inputFile);
        if (inputFile) {
            pixels.emplace_back(x, y);
            scales.emplace_back(tmpScale);
        }
    }


    while (inputFile) {
        float x = readNumber<float>(inputFile);
        float y = readNumber<float>(inputFile);
        int scale = readNumber<uint16_t>(inputFile);

        uint8_t r = readNumber<uint8_t>(inputFile);
        uint8_t g = readNumber<uint8_t>(inputFile);
        uint8_t b = readNumber<uint8_t>(inputFile);
        uint8_t a = readNumber<uint8_t>(inputFile);

        if (inputFile) {
            recoloredPixels.emplace_back(x, y);
            newScales.emplace_back(scale);
            newColors.push_back({ (int)r, (int)g, (int)b, (int)a });
        }
    }

    inputFile.close();

    // Изменение цвета пикселя в указанных координатах
    /*for (size_t i = 0; i < pixels.size(); ++i) {
        if (static_cast<int>(pixels[i].x) == x && static_cast<int>(pixels[i].y) == y) {
            palette[i % paletteEntries].r = r;
            palette[i % paletteEntries].g = g;
            palette[i % paletteEntries].b = b;
            palette[i % paletteEntries].a = a;
            break;
        }
    }*/

    // Запись в файл
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Не удалось открыть выходной файл для записи." << std::endl;
        return;
    }

    writeNumber(outputFile, width);
    writeNumber(outputFile, height);
    writeNumber(outputFile, bitsPerPixel);
    writeNumber(outputFile, paletteEntries);

    for (const auto& entry : palette) {
        writeNumber(outputFile, static_cast<uint16_t>(entry.angle));
        writeNumber(outputFile, static_cast<uint16_t>(entry.length));
        writeNumber(outputFile, static_cast<uint8_t>(entry.r));
        writeNumber(outputFile, static_cast<uint8_t>(entry.g));
        writeNumber(outputFile, static_cast<uint8_t>(entry.b));
        writeNumber(outputFile, static_cast<uint8_t>(entry.a));
    }

    for (size_t i = 0; i < pixels.size(); ++i) {
        writeNumber(outputFile, pixels[i].x);
        writeNumber(outputFile, pixels[i].y);
        writeNumber(outputFile, static_cast<uint16_t>(scales[i]));
    }

    for (size_t i = 0; i < recoloredPixels.size(); ++i) {
        writeNumber(outputFile, recoloredPixels[i].x);
        writeNumber(outputFile, recoloredPixels[i].y);
        writeNumber(outputFile, static_cast<uint16_t>(newScales[i]));

        writeNumber(outputFile, static_cast<uint8_t>(newColors[i][0]));
        writeNumber(outputFile, static_cast<uint8_t>(newColors[i][1]));
        writeNumber(outputFile, static_cast<uint8_t>(newColors[i][2]));
        writeNumber(outputFile, static_cast<uint8_t>(newColors[i][3]));
    }

    writeNumber(outputFile, _x);
    writeNumber(outputFile, _y);
    writeNumber(outputFile, static_cast<uint16_t>(scales[0]));

    writeNumber(outputFile, static_cast<uint8_t>(_r));
    writeNumber(outputFile, static_cast<uint8_t>(_g));
    writeNumber(outputFile, static_cast<uint8_t>(_b));
    writeNumber(outputFile, static_cast<uint8_t>(_a));

    outputFile.close();
}
// Функция для генерации узора
void generatePattern(const std::string& outputFilename, int width, int height, const std::string& patternType) {
    std::ofstream file(outputFilename, std::ios::binary);
    if (!file) {
        std::cerr << "Не удалось создать выходной файл." << std::endl;
        return;
    }

    int scale = 20;

    width = width * scale;
    height = height * scale;

    // Запись параметров изображения
    writeNumber(file, static_cast<uint16_t>(width));
    writeNumber(file, static_cast<uint16_t>(height));
    writeNumber(file, static_cast<uint8_t>(24)); 
    writeNumber(file, static_cast<uint16_t>(1)); 

    
    writeNumber(file, static_cast<uint16_t>(360));
    writeNumber(file, static_cast<uint16_t>(15));
    writeNumber(file, static_cast<uint8_t>(255));
    writeNumber(file, static_cast<uint8_t>(255));
    writeNumber(file, static_cast<uint8_t>(255));
    writeNumber(file, static_cast<uint8_t>(255));
    


    for (size_t i = 0; i < (width/scale) * (height/scale); ++i) {
        writeNumber(file, 0.0f);
        writeNumber(file, 0.0f);
        writeNumber(file, static_cast<uint16_t>(20));
    }



    // Генерация узора
    std::vector<sf::Vector2f> pixels;
    std::vector<int> scales;
    std::vector<std::vector<int>> colors;

    /*if (patternType == "tiles") {
        int tileSize = 20;
        for (int y = 0; y < height; y += 20) {
            for (int x = 0; x < width; x += 20) {
                pixels.emplace_back(static_cast<float>(x), static_cast<float>(y));
                scales.push_back(tileSize);
                colors.push_back({ (x / tileSize) % 2 == 0 ? 255 : 0, (y / tileSize) % 2 == 0 ? 255 : 0, 0, 255 });
            }
        }
    }
    else if (patternType == "circles") {
        int circleRadius = 20;
        for (int y = 0; y < height; y += circleRadius * 2) {
            for (int x = 0; x < width; x += circleRadius * 2) {
                pixels.emplace_back(static_cast<float>(x + circleRadius), static_cast<float>(y + circleRadius));
                scales.push_back(circleRadius);
                colors.push_back({ 255, 0, 0, 255 });
            }
        }
    }*/
    if (patternType == "gradient") {
        for (int y = 0; y < height / scale; ++y) {
            for (int x = 0; x < width / scale; ++x) {
                pixels.emplace_back(static_cast<float>(x), static_cast<float>(y));
                scales.push_back(scale);
                colors.push_back({ static_cast<int>(255 * x / (width / scale)), static_cast<int>(255 * y / (height / scale)), 0, 255 });
            }
        }
    }

    // Запись пикселей в файл
    for (size_t i = 0; i < pixels.size(); ++i) {
        writeNumber(file, pixels[i].x);
        writeNumber(file, pixels[i].y);
        writeNumber(file, static_cast<uint16_t>(scales[i]));
        writeNumber(file, static_cast<uint8_t>(colors[i][0]));
        writeNumber(file, static_cast<uint8_t>(colors[i][1]));
        writeNumber(file, static_cast<uint8_t>(colors[i][2]));
        writeNumber(file, static_cast<uint8_t>(colors[i][3]));
    }

    file.close();
}
// Функция для конвертации в JSON
void convertToJSON(const std::string& inputFilename, const std::string& outputFilename) {
    std::ifstream inputFile(inputFilename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Не удалось открыть исходный файл." << std::endl;
        return;
    }

    std::ofstream outputFile(outputFilename);
    if (!outputFile) {
        std::cerr << "Не удалось создать выходной файл." << std::endl;
        return;
    }

    // Чтение заголовка
    uint16_t width = readNumber<uint16_t>(inputFile);
    uint16_t height = readNumber<uint16_t>(inputFile);
    uint8_t bitsPerPixel = readNumber<uint8_t>(inputFile);
    uint16_t paletteEntries = readNumber<uint16_t>(inputFile);

    // Запись заголовка в JSON
    outputFile << "{\n";
    outputFile << "  \"Header\": {\n";
    outputFile << "    \"Width\": " << width << ",\n";
    outputFile << "    \"Height\": " << height << ",\n";
    outputFile << "    \"BitsPerPixel\": " << static_cast<int>(bitsPerPixel) << ",\n";
    outputFile << "    \"PaletteEntries\": " << paletteEntries << "\n";
    outputFile << "  },\n";

    // Запись палитры
    outputFile << "  \"Palette\": [\n";
    for (int i = 0; i < paletteEntries; ++i) {
        uint16_t angle = readNumber<uint16_t>(inputFile);
        uint16_t length = readNumber<uint16_t>(inputFile);
        uint8_t r = readNumber<uint8_t>(inputFile);
        uint8_t g = readNumber<uint8_t>(inputFile);
        uint8_t b = readNumber<uint8_t>(inputFile);
        uint8_t a = readNumber<uint8_t>(inputFile);

        outputFile << "    {\n";
        outputFile << "      \"Angle\": " << angle << ",\n";
        outputFile << "      \"Length\": " << length << ",\n";
        outputFile << "      \"Color\": { \"r\": " << static_cast<int>(r)
            << ", \"g\": " << static_cast<int>(g)
            << ", \"b\": " << static_cast<int>(b)
            << ", \"a\": " << static_cast<int>(a) << " }\n";
        outputFile << "    }";
        if (i < paletteEntries - 1) outputFile << ",";
        outputFile << "\n";
    }
    outputFile << "  ],\n";

    // Запись пикселей
    outputFile << "  \"Pixels\": [\n";
    bool firstPixel = true;

    float x = readNumber<float>(inputFile);
    float y = readNumber<float>(inputFile);
    uint16_t scale = readNumber<uint16_t>(inputFile);

    outputFile << "    { \"x\": " << x << ", \"y\": " << y
        << ", \"scale\": " << scale
        << " }";

    firstPixel = false;


    for (size_t i = 0; i < ((width / scale) * (height / scale)) - 1; ++i) {
        float x = readNumber<float>(inputFile);
        float y = readNumber<float>(inputFile);
        uint16_t scale = readNumber<uint16_t>(inputFile);

        if (!firstPixel) outputFile << ",\n";
        outputFile << "    { \"x\": " << x << ", \"y\": " << y
            << ", \"scale\": " << scale << " }";
    }
    outputFile << "\n  ],\n";
    

    outputFile << "  \"RecoloredPixels\": [\n";

    firstPixel = true;
    while (inputFile) {
        float x = readNumber<float>(inputFile);
        float y = readNumber<float>(inputFile);
        uint16_t scale = readNumber<uint16_t>(inputFile);
        uint8_t r = readNumber<uint8_t>(inputFile);
        uint8_t g = readNumber<uint8_t>(inputFile);
        uint8_t b = readNumber<uint8_t>(inputFile);
        uint8_t a = readNumber<uint8_t>(inputFile);

        if (inputFile) {
            if (!firstPixel) outputFile << ",\n";
            outputFile << "    { \"x\": " << x << ", \"y\": " << y
                << ", \"scale\": " << scale << ", \"r\": " << static_cast<int>(r)
                << ", \"g\": " << static_cast<int>(g)
                << ", \"b\": " << static_cast<int>(b)
                << ", \"a\": " << static_cast<int>(a) << " }";
        }
        firstPixel = false;
    }
    outputFile << "\n  ]\n";
    outputFile << "}\n";

    inputFile.close();
    outputFile.close();
}

// Основной код UI и логики
int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Lab2_GUI", sf::Style::Default);
    ImGui::SFML::Init(window);

    std::string filename = "";
    std::string TmpFilename = "";
    std::string inputFilename = "";
    std::string outputFilename = "";

    int scale = 20;

    bool renderFile = false;  // Флаг для рендеринга

    bool CreateFile = false;
    bool ReadFile = false;
    bool ScaleFile = false;
    bool AdjustContrast = false;
    bool ChangePixelColor = false;
    bool GeneratePattern = false;
    bool ConvertToJSON = false;
    bool ChangePallet = false;

    bool isPalletSet = false;
    bool isSizeSet = false;
    bool isPalletLoad = false;

    bool pixelsNotEmpty = false;

    bool isValidExtension = true;

    bool isValidInputName = true;
    bool isValidOutputName = true;

    bool isValidName = true;

    float contrastFactor = 1.0f;

    int x = 0, y = 0, r = 255, g = 255, b = 255, a = 255;

    std::vector<sf::RectangleShape> pixelShapes;
    uint16_t imageWidth = 0, imageHeight = 0;

    // Данные для создания файла
    int width = 1000, height = 1000, bitsPerPixel = 24, paletteEntries = 0;
    std::vector<PaletteEntry> palette;
    std::vector<sf::Vector2f> pixels;
    std::vector<int> scales;


    uint16_t _Scale;
    // Чтение пикселей
    //std::vector<sf::Vector2f> pixels;
    std::vector<int> newScales;
    std::vector<sf::Vector2f> recoloredPixels;
    std::vector<std::vector<int>> newColors;

    // Основной цикл приложения
    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) window.close();
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        window.clear(sf::Color::Black);

        if (renderFile) {
            for (const auto& shape : pixelShapes) {
                window.draw(shape);
            }

            // Кнопка возврата в меню
            ImGui::Begin("Back");
            if (ImGui::Button("Back to menu")) {
                renderFile = false;  // Отключаем рендеринг
                ReadFile = false;
                CreateFile = false;
                ScaleFile = false;
                AdjustContrast = false;
                pixelShapes.clear(); // Очищаем пиксели
            }
            ImGui::End();
        }
        else {
            // Основное меню
            if (!CreateFile && !ReadFile && !ScaleFile && !AdjustContrast && !ChangePixelColor && !GeneratePattern && !ConvertToJSON && !ChangePallet && !isPalletLoad) {
                ImGui::Begin("Main Menu");
                if (ImGui::Button("Create File")) {
                    // Сбрасываем данные для создания файла
                    width = height = 1000;
                    bitsPerPixel = 24;
                    paletteEntries = 0;
                    outputFilename = "example.ya3";
                    palette.clear();
                    pixels.clear();
                    scales.clear();
                    newScales.clear();
                    recoloredPixels.clear();
                    newColors.clear();

                    CreateFile = true;
                }
                if (ImGui::Button("Read File")) {
                    filename = "example.ya3";
                    ReadFile = true;
                }
                if (ImGui::Button("Scale File")) {
                    outputFilename = "example.ya3";
                    inputFilename = "example.ya3";
                    scale = 1;
                    ScaleFile = true;
                }
                if (ImGui::Button("Adjust Contrast")) {
                    outputFilename = "example.ya3";
                    inputFilename = "example.ya3";
                    AdjustContrast = true;
                }
                if (ImGui::Button("Change Pixel Color")) {
                    outputFilename = "example.ya3";
                    inputFilename = "example.ya3";
                    ChangePixelColor = true;
                }
                if (ImGui::Button("Generate Pattern")) {
                    outputFilename = "example.ya3";
                    width = 1000;
                    height = 1000;
                    GeneratePattern = true;
                }
                if (ImGui::Button("Convert To JSON")) {
                    inputFilename = "example.ya3";
                    outputFilename = "example.json";
                    ConvertToJSON = true;
                }
                if (ImGui::Button("Change Palette")) {
                    inputFilename = "example.ya3";
                    outputFilename = "example.json";
                    ChangePallet = true;
                    palette.clear();
                    pixels.clear();
                    scales.clear();
                    newScales.clear();
                    recoloredPixels.clear();
                    newColors.clear();
                }
                ImGui::End();
            }

            // Если пользователь выбрал создание файла
            if (CreateFile == true) {
                ImGui::Begin("Create File");
                if (ImGui::Button("Back to menu")) {
                    renderFile = false;  // Отключаем рендеринг
                    ReadFile = false;
                    CreateFile = false;
                    ScaleFile = false;
                    AdjustContrast = false;
                    pixelShapes.clear(); // Очищаем пиксели
                }

                ImGui::InputText("Output File Name", &outputFilename[0], outputFilename.size() + 1);
                if (!isValidOutputName) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a valid filename with '.ya3' extension.");
                }
                if (ImGui::InputInt("Width", &width)) {
                    isSizeSet = false;
                }
                if (ImGui::InputInt("Height", &height)) {
                    isSizeSet = false;
                }
                if (ImGui::Button("Set Size of Image")) {
                    pixels.resize(width * height);
                    for (size_t i = 0; i < width * height; i++) {
                        pixels[i].x = 0.0f;
                        pixels[i].y = 0.0f;
                    }
                    isSizeSet = true;
                }
                //ImGui::InputInt("Bits Per Pixel", &bitsPerPixel);
                if (ImGui::InputInt("Palette Entries", &paletteEntries)) {
                    isPalletSet = false;
                }

                if (ImGui::Button("Set Palette Entries")) {
                    palette.resize(paletteEntries);
                    isPalletSet = true;
                }

                if (isPalletSet == true) {
                    for (int i = 0; i < paletteEntries; ++i) {
                        ImGui::Text("Palette Entry %d", i + 1);

                        // Уникальные метки для каждого элемента
                        ImGui::InputInt(("Angle " + std::to_string(i + 1)).c_str(), &palette[i].angle);
                        ImGui::InputInt(("R " + std::to_string(i + 1)).c_str(), &palette[i].r);
                        ImGui::InputInt(("G " + std::to_string(i + 1)).c_str(), &palette[i].g);
                        ImGui::InputInt(("B " + std::to_string(i + 1)).c_str(), &palette[i].b);
                        ImGui::InputInt(("A " + std::to_string(i + 1)).c_str(), &palette[i].a);
                    }
                }

                int scale = 20;

                if (isSizeSet) {
                    for (int i = 0; i < width * height; ++i) {
                        ImGui::Text("Pixel %d", i + 1);

                        ImGui::InputFloat(("X " + std::to_string(i + 1)).c_str(), &pixels[i].x);
                        ImGui::InputFloat(("Y " + std::to_string(i + 1)).c_str(), &pixels[i].y);
                        scales.push_back(scale);
                    }
                }

                if (pixelsNotEmpty == true) {
                    ImGui::Text("Pixel Coordinates:");
                    for (size_t i = 0; i < pixels.size(); ++i) {
                        ImGui::Text("Pixel %d: (%.2f, %.2f)", i + 1, pixels[i].x, pixels[i].y);
                    }
                }

                if (ImGui::Button("Done!")) {
                    std::string tmpString2;
                    for (int i = 0; i < outputFilename.length() - 4; i++) {
                        if (outputFilename[i] == '.' && outputFilename[i + 1] == 'y' && outputFilename[i + 2] == 'a' && outputFilename[i + 3] == '3') {
                            tmpString2.append(".ya3");
                            break;
                        }
                        tmpString2.push_back(outputFilename[i]);
                    }

                    isValidOutputName = tmpString2.length() >= 5 && tmpString2.substr(tmpString2.length() - 4) == ".ya3";

                    if (isValidOutputName) {
                        width = width * scale;
                        height = height * scale;

                        // Запись в файл
                        std::ofstream file(outputFilename, std::ios::binary);
                        if (file) {
                            writeNumber(file, (uint16_t)width);
                            writeNumber(file, (uint16_t)height);
                            writeNumber(file, (uint8_t)bitsPerPixel);
                            writeNumber(file, (uint16_t)paletteEntries);

                            for (const auto& entry : palette) {
                                writeNumber(file, (uint16_t)entry.angle);
                                writeNumber(file, (uint16_t)entry.length);
                                writeNumber(file, (uint8_t)entry.r);
                                writeNumber(file, (uint8_t)entry.g);
                                writeNumber(file, (uint8_t)entry.b);
                                writeNumber(file, (uint8_t)entry.a);
                            }

                            for (size_t i = 0; i < pixels.size(); ++i) {
                                writeNumber(file, pixels[i].x);
                                writeNumber(file, pixels[i].y);
                                writeNumber(file, (uint16_t)scales[i]);
                            }

                            file.close();

                            pixelShapes = loadFile(outputFilename, imageWidth, imageHeight);
                            renderFile = true;  // Включаем рендеринг
                        }
                    }
                }
                ImGui::End();
            }

            if (ScaleFile) {
                ImGui::Begin("Scale File");

                if (ImGui::Button("Back to menu")) {
                    renderFile = false;  // Отключаем рендеринг
                    ReadFile = false;
                    CreateFile = false;
                    ScaleFile = false;
                    AdjustContrast = false;
                    pixelShapes.clear(); // Очищаем пиксели
                }

                ImGui::InputText("Input File Name", &inputFilename[0], inputFilename.size() + 1);
                if (!isValidInputName) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a valid filename with '.ya3' extension.");
                }
                ImGui::InputText("Output File Name", &outputFilename[0], outputFilename.size() + 1);
                if (!isValidOutputName) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a valid filename with '.ya3' extension.");
                }
                ImGui::InputInt("Scale Factor", &scale);
                if (ImGui::Button("Done!")) {
                    std::string tmpString;
                    for (int i = 0; i < inputFilename.length() - 4; i++) {
                        if (inputFilename[i] == '.' && inputFilename[i + 1] == 'y' && inputFilename[i + 2] == 'a' && inputFilename[i + 3] == '3') {
                            tmpString.append(".ya3");
                            break;
                        }
                        tmpString.push_back(inputFilename[i]);
                    }

                    isValidInputName = tmpString.length() >= 5 && tmpString.substr(tmpString.length() - 4) == ".ya3";

                    std::string tmpString2;
                    for (int i = 0; i < outputFilename.length() - 4; i++) {
                        if (outputFilename[i] == '.' && outputFilename[i + 1] == 'y' && outputFilename[i + 2] == 'a' && outputFilename[i + 3] == '3') {
                            tmpString2.append(".ya3");
                            break;
                        }
                        tmpString2.push_back(outputFilename[i]);
                    }

                    isValidOutputName = tmpString2.length() >= 5 && tmpString2.substr(tmpString2.length() - 4) == ".ya3";

                    if (isValidInputName && isValidOutputName) {
                        scaleImage(tmpString, tmpString2, scale);
                        pixelShapes = loadFile(tmpString2, imageWidth, imageHeight);
                        ScaleFile = false;
                        renderFile = true;  // Включаем рендеринг
                    }
                }

                ImGui::End();
            }

            if (AdjustContrast) {
                ImGui::Begin("Adjust Contrast");

                if (ImGui::Button("Back to menu")) {
                    renderFile = false;  // Отключаем рендеринг
                    ReadFile = false;
                    CreateFile = false;
                    ScaleFile = false;
                    AdjustContrast = false;
                    pixelShapes.clear(); // Очищаем пиксели
                }

                ImGui::InputText("Input File Name", &inputFilename[0], inputFilename.size() + 1);
                if (!isValidInputName) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a valid filename with '.ya3' extension.");
                }
                ImGui::InputText("Output File Name", &outputFilename[0], outputFilename.size() + 1);
                if (!isValidOutputName) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a valid filename with '.ya3' extension.");
                }
                
                ImGui::InputFloat("Contrast Factor", &contrastFactor);
                if (ImGui::Button("Done!")) {
                    std::string tmpString;
                    for (int i = 0; i < inputFilename.length() - 4; i++) {
                        if (inputFilename[i] == '.' && inputFilename[i + 1] == 'y' && inputFilename[i + 2] == 'a' && inputFilename[i + 3] == '3') {
                            tmpString.append(".ya3");
                            break;
                        }
                        tmpString.push_back(inputFilename[i]);
                    }

                    isValidInputName = tmpString.length() >= 5 && tmpString.substr(tmpString.length() - 4) == ".ya3";

                    std::string tmpString2;
                    for (int i = 0; i < outputFilename.length() - 4; i++) {
                        if (outputFilename[i] == '.' && outputFilename[i + 1] == 'y' && outputFilename[i + 2] == 'a' && outputFilename[i + 3] == '3') {
                            tmpString2.append(".ya3");
                            break;
                        }
                        tmpString2.push_back(outputFilename[i]);
                    }

                    isValidOutputName = tmpString2.length() >= 5 && tmpString2.substr(tmpString2.length() - 4) == ".ya3";

                    if (isValidInputName && isValidOutputName) {
                        adjustContrast(tmpString, tmpString2, contrastFactor);
                        pixelShapes = loadFile(tmpString2, imageWidth, imageHeight);
                        AdjustContrast = false;
                        renderFile = true;  // Включаем рендеринг
                    }
                }

                ImGui::End();
            }

            if (ChangePixelColor) {
                ImGui::Begin("Change Pixel Color");

                if (ImGui::Button("Back to menu")) {
                    renderFile = false;  // Отключаем рендеринг
                    ReadFile = false;
                    CreateFile = false;
                    ScaleFile = false;
                    AdjustContrast = false;
                    ChangePixelColor = false;
                    pixelShapes.clear(); // Очищаем пиксели
                }

                ImGui::InputText("File Name", &inputFilename[0], inputFilename.size() + 1);
                if (!isValidName) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a valid filename with '.ya3' extension.");
                }

               
                ImGui::InputInt("X Coordinate", &x);
                ImGui::InputInt("Y Coordinate", &y);
                ImGui::InputInt("Red (R)", &r);
                ImGui::InputInt("Green (G)", &g);
                ImGui::InputInt("Blue (B)", &b);
                ImGui::InputInt("Alpha (A)", &a);

                if (ImGui::Button("Change Color")) {
                    /*std::string tmpString;
                    for (int i = 0; i < filename.length() - 4; i++) {
                        if (filename[i] == '.' && filename[i + 1] == 'y' && filename[i + 2] == 'a' && filename[i + 3] == '3') {
                            tmpString.append(".ya3");
                            break;
                        }
                        tmpString.push_back(filename[i]);
                    }

                    isValidName = tmpString.length() >= 5 && tmpString.substr(tmpString.length() - 4) == ".ya3";*/

                    isValidName = true;

                    if (isValidName) {
                        changePixelColor(inputFilename, x, y, r, g, b, a);
                        pixelShapes = loadFile(inputFilename, imageWidth, imageHeight);
                        ChangePixelColor = false;
                        renderFile = true;  // Включаем рендеринг
                    }
                }

                ImGui::End();
            }

            if (GeneratePattern) {
                ImGui::Begin("Generate Pattern");

                if (ImGui::Button("Back to menu")) {
                    renderFile = false;  // Отключаем рендеринг
                    ReadFile = false;
                    CreateFile = false;
                    ScaleFile = false;
                    AdjustContrast = false;
                    GeneratePattern = false;
                    pixelShapes.clear(); // Очищаем пиксели
                }

                ImGui::InputText("Output File Name", &outputFilename[0], outputFilename.size() + 1);
                if (!isValidOutputName) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a valid filename with '.ya3' extension.");
                }

                if (ImGui::InputInt("Width", &width)) {
                    isSizeSet = false;
                }
                if (ImGui::InputInt("Height", &height)) {
                    isSizeSet = false;
                }

                static const char* patternTypes[] = { "gradient" };
                static int currentPatternType = 0;
                ImGui::Combo("Pattern Type", &currentPatternType, patternTypes, IM_ARRAYSIZE(patternTypes));

                if (ImGui::Button("Generate")) {
                    std::string tmpString2;
                    for (int i = 0; i < outputFilename.length() - 4; i++) {
                        if (outputFilename[i] == '.' && outputFilename[i + 1] == 'y' && outputFilename[i + 2] == 'a' && outputFilename[i + 3] == '3') {
                            tmpString2.append(".ya3");
                            break;
                        }
                        tmpString2.push_back(outputFilename[i]);
                    }

                    isValidOutputName = tmpString2.length() >= 5 && tmpString2.substr(tmpString2.length() - 4) == ".ya3";

                    if (isValidOutputName) {
                        generatePattern(tmpString2, width, height, patternTypes[currentPatternType]);
                        pixelShapes = loadFile(tmpString2, imageWidth, imageHeight);
                        GeneratePattern = false;
                        renderFile = true;  // Включаем рендеринг
                    }
                }

                ImGui::End();
            }

            if (ConvertToJSON) {
                ImGui::Begin("Convert to JSON");
                if (ImGui::Button("Back to menu")) {
                    renderFile = false;  // Отключаем рендеринг
                    ReadFile = false;
                    CreateFile = false;
                    ScaleFile = false;
                    AdjustContrast = false;
                    GeneratePattern = false;
                    ConvertToJSON = false;
                    pixelShapes.clear(); // Очищаем пиксели
                }

                ImGui::InputText("Input File Name", &inputFilename[0], inputFilename.size() + 1);
                if (!isValidInputName) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a valid filename with '.ya3' extension.");
                }
                ImGui::InputText("Output File Name", &outputFilename[0], outputFilename.size() + 1);
                if (!isValidOutputName) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a valid filename with '.ya3' extension.");
                }

                
                if (ImGui::Button("Done!")) {
                    std::string tmpString;
                    for (int i = 0; i < inputFilename.length() - 4; i++) {
                        if (inputFilename[i] == '.' && inputFilename[i + 1] == 'y' && inputFilename[i + 2] == 'a' && inputFilename[i + 3] == '3') {
                            tmpString.append(".ya3");
                            break;
                        }
                        tmpString.push_back(inputFilename[i]);
                    }

                    isValidInputName = tmpString.length() >= 5 && tmpString.substr(tmpString.length() - 4) == ".ya3";

                    std::string tmpString2;
                    for (int i = 0; i < outputFilename.length() - 5; i++) {
                        if (outputFilename[i] == '.' && outputFilename[i + 1] == 'j' && outputFilename[i + 2] == 's' && outputFilename[i + 3] == 'o' && outputFilename[i + 4] == 'n') {
                            tmpString2.append(".json");
                            break;
                        }
                        tmpString2.push_back(outputFilename[i]);
                    }

                    isValidOutputName = tmpString2.length() >= 5 && tmpString2.substr(tmpString2.length() - 5) == ".json";

                    if (isValidInputName && isValidOutputName) {
                        convertToJSON(tmpString, tmpString2);
                        ConvertToJSON = false;
                    }
                }

                ImGui::End();
            }

            if (ChangePallet) {
                ImGui::Begin("ChangePallet");

                if (ImGui::Button("Back to menu")) {
                    renderFile = false;  // Отключаем рендеринг
                    ReadFile = false;
                    CreateFile = false;
                    ScaleFile = false;
                    AdjustContrast = false;
                    GeneratePattern = false;
                    ConvertToJSON = false;
                    ChangePallet = false;
                    pixelShapes.clear(); // Очищаем пиксели
                }

                ImGui::InputText("Input File Name", &inputFilename[0], inputFilename.size() + 1);
                if (!isValidInputName) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a valid filename with '.ya3' extension.");
                }

                if (ImGui::Button("Done!")) {
                    std::string tmpString;
                    for (int i = 0; i < inputFilename.length() - 4; i++) {
                        if (inputFilename[i] == '.' && inputFilename[i + 1] == 'y' && inputFilename[i + 2] == 'a' && inputFilename[i + 3] == '3') {
                            tmpString.append(".ya3");
                            break;
                        }
                        tmpString.push_back(inputFilename[i]);
                    }

                    isValidInputName = tmpString.length() >= 5 && tmpString.substr(tmpString.length() - 4) == ".ya3";

                    if (isValidInputName) {
                        ChangePallet = false;

                        std::ifstream file(tmpString, std::ios::binary);

                        width = readNumber<uint16_t>(file);
                        height = readNumber<uint16_t>(file);
                        bitsPerPixel = readNumber<uint8_t>(file);
                        paletteEntries = readNumber<uint16_t>(file);

                        palette.resize(paletteEntries);

                        for (auto& entry : palette) {
                            entry.angle = readNumber<uint16_t>(file);
                            entry.length = readNumber<uint16_t>(file);
                            entry.r = readNumber<uint8_t>(file);
                            entry.g = readNumber<uint8_t>(file);
                            entry.b = readNumber<uint8_t>(file);
                            entry.a = readNumber<uint8_t>(file);
                        }
                        

                        float x = readNumber<float>(file);
                        float y = readNumber<float>(file);
                        uint16_t tmpScale = readNumber<uint16_t>(file);
                        if (file) {
                            pixels.emplace_back(x, y);
                            _Scale = tmpScale;
                        }

                        for (size_t i = 0; i < ((width / scale) * (height / scale)) - 1; ++i) {
                            float x = readNumber<float>(file);
                            float y = readNumber<float>(file);
                            uint16_t tmpScale = readNumber<uint16_t>(file);
                            if (file) {
                                pixels.emplace_back(x, y);
                                _Scale = tmpScale;
                            }
                        }

                        while (file) {
                            float x = readNumber<float>(file);
                            float y = readNumber<float>(file);
                            int tmscale = readNumber<uint16_t>(file);

                            int r = readNumber<uint8_t>(file);
                            int g = readNumber<uint8_t>(file);
                            int b = readNumber<uint8_t>(file);
                            int a = readNumber<uint8_t>(file);

                            if (file) {
                                recoloredPixels.emplace_back(x, y);
                                newScales.emplace_back(tmscale);
                                newColors.push_back({ r, g, b, a });
                            }
                        }
                        file.close();

                        outputFilename = tmpString;
                        isPalletLoad = true;
                    }
                }


                ImGui::End();
            }

            if (isPalletLoad) {
                ImGui::Begin("Change Pallet");

                for (size_t i = 0; i < paletteEntries; ++i){
                    ImGui::Text("Palette Entry %d", i + 1);

                    // Уникальные метки для каждого элемента
                    ImGui::InputInt(("Angle " + std::to_string(i + 1)).c_str(), &palette[i].angle);
                    ImGui::InputInt(("R " + std::to_string(i + 1)).c_str(), &palette[i].r);
                    ImGui::InputInt(("G " + std::to_string(i + 1)).c_str(), &palette[i].g);
                    ImGui::InputInt(("B " + std::to_string(i + 1)).c_str(), &palette[i].b);
                    ImGui::InputInt(("A " + std::to_string(i + 1)).c_str(), &palette[i].a);
                }
                
                if (ImGui::Button("Done!")) {

                    std::ofstream file(outputFilename, std::ios::binary);
                    if (file) {
                        writeNumber(file, (uint16_t)width);
                        writeNumber(file, (uint16_t)height);
                        writeNumber(file, (uint8_t)bitsPerPixel);
                        writeNumber(file, (uint16_t)paletteEntries);

                        for (const auto& entry : palette) {
                            writeNumber(file, (uint16_t)entry.angle);
                            writeNumber(file, (uint16_t)entry.length);
                            writeNumber(file, (uint8_t)entry.r);
                            writeNumber(file, (uint8_t)entry.g);
                            writeNumber(file, (uint8_t)entry.b);
                            writeNumber(file, (uint8_t)entry.a);
                        }

                        for (size_t i = 0; i < pixels.size(); ++i) {
                            writeNumber(file, pixels[i].x);
                            writeNumber(file, pixels[i].y);
                            writeNumber(file, static_cast<uint16_t>(_Scale));
                        }

                        for (size_t i = 0; i < recoloredPixels.size(); ++i) {
                            writeNumber(file, recoloredPixels[i].x);
                            writeNumber(file, recoloredPixels[i].y);
                            writeNumber(file, static_cast<uint16_t>(_Scale));

                            writeNumber(file, static_cast<uint8_t>(newColors[i][0]));
                            writeNumber(file, static_cast<uint8_t>(newColors[i][1]));
                            writeNumber(file, static_cast<uint8_t>(newColors[i][2]));
                            writeNumber(file, static_cast<uint8_t>(newColors[i][3]));
                        }

                        file.close();

                        pixelShapes = loadFile(outputFilename, imageWidth, imageHeight);
                        renderFile = true;  // Включаем рендеринг
                    }


                    isPalletLoad = false;
                }

                ImGui::End();
            }

            // Если пользователь выбрал чтение файла
            if (ReadFile) {
                ImGui::Begin("Read File");

                if (ImGui::Button("Back to menu")) {
                    renderFile = false;  // Отключаем рендеринг
                    ReadFile = false;
                    CreateFile = false;
                    ScaleFile = false;
                    AdjustContrast = false;
                    pixelShapes.clear(); // Очищаем пиксели
                }

                ImGui::InputText("File Name", &filename[0], filename.size() + 1);

                if (ImGui::Button("Done!")) {
                    std::string tmpString;
                    bool fl = true;
                    for (int i = 0; i < filename.length() - 4; i++) {
                        if (filename[i] == '.' && filename[i + 1] == 'y' && filename[i + 2] == 'a' && filename[i + 3] == '3') {
                            tmpString.append(".ya3");
                            break;
                        }
                        tmpString.push_back(filename[i]);
                    }

                    isValidName = tmpString.length() >= 5 && tmpString.substr(tmpString.length() - 4) == ".ya3";
                    int ts = tmpString.length();
                    if (isValidName) {
                        std::ifstream file1(filename, std::ios::binary);
                        if (!file1) {
                            isValidName = false;
                        }
                        else {
                            imageWidth = readNumber<uint16_t>(file1);
                            imageHeight = readNumber<uint16_t>(file1);
                            file1.close();
                            pixelShapes = loadFile(tmpString, imageWidth, imageHeight);
                            renderFile = true;  // Включаем рендеринг
                        }
                    }
                }

                if (isValidName == false) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "File not exist OR Please enter a valid filename with '.ya3' extension.");
                }

                ImGui::End();
            }
        }

        ImGui::SFML::Render(window);
        ImGui::Render();
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}