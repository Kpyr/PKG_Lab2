/*============================================
    Групповой вариант #4



============================================*/









//#include <iostream>
//#include <fstream>
//#include <vector>
//#include <SFML/Graphics.hpp>
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
//        std::cerr << "Не удалось открыть файл для чтения." << std::endl;
//        return 1;
//    }
//
//    // Читаем ширину и высоту (2 байта каждое)
//    uint16_t width = readNumber<uint16_t>(file);
//    uint16_t height = readNumber<uint16_t>(file);
//
//    // Читаем количество бит на пиксель (1 байт)
//    uint8_t bitsPerPixel = readNumber<uint8_t>(file);
//
//    // Читаем количество значений палитры (2 байта)
//    uint16_t paletteEntries = readNumber<uint16_t>(file);
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
//    // Закрываем файл
//    file.close();
//
//    // Создаем окно SFML
//    sf::RenderWindow window(sf::VideoMode(width, height), "SFML Circle");
//
//    // Радиус круга - наименьшая величина из высоты и ширины
//    float radius = std::min(width, height) / 2.0f - 50.0f;
//
//    // Центр окна
//    sf::Vector2f center(width / 2.0f, height / 2.0f);
//
//    // Создаем сектора
//    std::vector<sf::VertexArray> sectors(paletteEntries, sf::VertexArray(sf::TrianglesFan));
//
//    float totalAngle = 0.0f;
//    for (size_t i = 0; i < paletteEntries; ++i) {
//        float angle = angles[i] * 3.14159265f / 180.0f;
//        float length = lengths[i];
//        sf::Color color(rValues[i], gValues[i], bValues[i], aValues[i]);
//
//        // Добавляем центральную точку сектора
//        sectors[i].append(sf::Vertex(center, color));
//
//        // Добавляем вершины для дуги сектора
//        for (float a = 0.0f; a <= angle; a += 0.01f) {
//            sectors[i].append(sf::Vertex(center + radius * sf::Vector2f(std::cos(totalAngle + a), std::sin(totalAngle + a)), color));
//        }
//
//        totalAngle += angle;
//    }
//
//    // Основной цикл приложения
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed)
//                window.close();
//        }
//
//        window.clear(sf::Color::White);
//
//        // Рисуем сектора
//        for (const auto& sector : sectors) {
//            window.draw(sector);
//        }
//
//        window.display();
//    }
//
//    return 0;
//}




#include <iostream>
#include <fstream>
#include <vector>
#include <SFML/Graphics.hpp>

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
        std::cerr << "Не удалось открыть файл для чтения." << std::endl;
        return 1;
    }

    // Читаем ширину и высоту (2 байта каждое)
    uint16_t width = readNumber<uint16_t>(file);
    uint16_t height = readNumber<uint16_t>(file);

    // Читаем количество бит на пиксель (1 байт)
    uint8_t bitsPerPixel = readNumber<uint8_t>(file);

    // Читаем количество значений палитры (2 байта)
    uint16_t paletteEntries = readNumber<uint16_t>(file);

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

    // Закрываем файл
    file.close();

    // Создаем окно SFML
    sf::RenderWindow window(sf::VideoMode(width, height), "SFML Circle");

    // Радиус круга - наименьшая величина из высоты и ширины
    float radius = std::min(width, height) / 2.0f - 50.0f;

    // Центр окна
    sf::Vector2f center(width / 2.0f, height / 2.0f);

    // Создаем контур круга
    sf::CircleShape circle(radius);
    circle.setPosition(center - sf::Vector2f(radius, radius));
    circle.setFillColor(sf::Color::Transparent);
    circle.setOutlineThickness(2.0f);
    circle.setOutlineColor(sf::Color::Black);

    // Основной цикл приложения
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);

        // Рисуем контур круга
        window.draw(circle);

        // Рисуем пиксели в зависимости от цвета сектора
        for (size_t i = 0; i < xValues.size(); ++i) {
            float x = xValues[i] + center.x;
            float y = yValues[i] + center.y;

            // Вычисляем угол и расстояние от центра до пикселя
            sf::Vector2f pixelPos(x, y);
            sf::Vector2f delta = pixelPos - center;
            float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
            float angle = std::atan2(delta.y, delta.x);

            // Определяем, в каком секторе находится пиксель
            int sectorIndex = -1;
            float totalAngle = 0.0f;
            for (size_t j = 0; j < paletteEntries; ++j) {
                if (angle >= totalAngle && angle < totalAngle + angles[j] * 3.14159265f / 180.0f) {
                    sectorIndex = j;
                    break;
                }
                totalAngle += angles[j] * 3.14159265f / 180.0f;
            }

            // Если пиксель находится внутри круга и в одном из секторов, рисуем его
            if (sectorIndex != -1 && distance <= radius) {
                sf::Color color(rValues[sectorIndex], gValues[sectorIndex], bValues[sectorIndex], aValues[sectorIndex]);
                sf::CircleShape pixel(2.0f);
                pixel.setPosition(x - 1.0f, y - 1.0f);
                pixel.setFillColor(color);
                window.draw(pixel);
            }
            else {
                std::cout << "Pixel: " << x << " " << y << std::endl;
            }
        }

        window.display();
    }

    return 0;
}