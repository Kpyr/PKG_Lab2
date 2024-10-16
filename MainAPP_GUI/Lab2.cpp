#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <cmath>

// Функция для записи числа в формате little-endian
template<typename T>
void writeNumber(std::ofstream& file, T value) {
    char* ptr = reinterpret_cast<char*>(&value);
    for (size_t i = 0; i < sizeof(T); ++i) {
        file.write(ptr + i, 1);
    }
}

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

struct PaletteEntry {
    int angle;
    int length;
    int r, g, b, a;
};

// Функция для загрузки пикселей из файла
std::vector<sf::CircleShape> loadFile(const std::string& filename, uint16_t& width, uint16_t& height) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<sf::CircleShape> pixelShapes;

    

    if (!file) {
        std::cerr << "Не удалось открыть файл." << std::endl;
        return pixelShapes;
    }

    // Чтение параметров изображения
    width = readNumber<uint16_t>(file);
    height = readNumber<uint16_t>(file);
    uint8_t bitsPerPixel = readNumber<uint8_t>(file);
    uint16_t paletteEntries = readNumber<uint16_t>(file);

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
    while (file) {
        float x = readNumber<float>(file);
        float y = readNumber<float>(file);
        if (file) pixels.emplace_back(x, y);
    }
    file.close();

    // Создание фигур для пикселей
    sf::Vector2f center(width / 2.0f, height / 2.0f);
    for (const auto& pixel : pixels) {
        sf::Vector2f pos = pixel + center;
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

            /*if (i == 2) {
                std::cout <<"ID: " << j << " " << angle << ' ' << totalAngle << " " << sectorAngle << " " << angles[j] << std::endl;
            } */
            if (angle >= totalAngle && angle < totalAngle + sectorAngle + 1e-5f) {
                sectorIndex = j;
                break;
            }
            totalAngle += sectorAngle;

        }

        float radius = std::min(width, height) / 2.0f - 50.0f;

        if (sectorIndex != -1 && distance <= radius){
            sf::CircleShape shape(2.0f);
            shape.setPosition(pos - sf::Vector2f(1.0f, 1.0f));
            sf::Color color(rValues[sectorIndex], gValues[sectorIndex], bValues[sectorIndex], aValues[sectorIndex]);
            //sf::Color color(255,0,0,255);
            shape.setFillColor(color);  // Тестовый цвет
            pixelShapes.push_back(shape);
        }
    }

    return pixelShapes;
}

// Основной код UI и логики
int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Lab2_GUI", sf::Style::Default);
    ImGui::SFML::Init(window);

    std::string filename = "test.ya3";
    bool renderFile = false;  // Флаг для рендеринга

    bool CreateFile = false;
    bool ReadFile = false;

    bool isPalletSet = false;

    bool pixelsNotEmpty = false;

    std::vector<sf::CircleShape> pixelShapes;
    uint16_t imageWidth = 0, imageHeight = 0;

    // Данные для создания файла
    //uint16_t width = 1000, height = 1000, bitsPerPixel = 24, paletteEntries = 0;
    int width = 1000, height = 1000, bitsPerPixel = 24, paletteEntries = 0;
    std::vector<PaletteEntry> palette;
    std::vector<sf::Vector2f> pixels;

    // Основной цикл приложения
    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) window.close();
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        window.clear(sf::Color::White);

        if (renderFile) {
            // Отрисовка изображения
            sf::CircleShape circle(std::min(imageWidth, imageHeight) / 2.0f - 50.0f);
            circle.setPosition(imageWidth / 2.0f - circle.getRadius(), imageHeight / 2.0f - circle.getRadius());
            circle.setOutlineThickness(2.0f);
            circle.setOutlineColor(sf::Color::Black);
            window.draw(circle);

            for (const auto& shape : pixelShapes) {
                window.draw(shape);
            }

            // Кнопка возврата в меню
            ImGui::Begin("Back");
            if (ImGui::Button("Back to menu")) {
                renderFile = false;  // Отключаем рендеринг
                ReadFile = false;
                CreateFile = false;
                pixelShapes.clear(); // Очищаем пиксели
            }
            ImGui::End();
        }
        else {
            // Основное меню
            if (!CreateFile && !ReadFile) {
                ImGui::Begin("Main Menu");
                if (ImGui::Button("Create File")) {
                    // Сбрасываем данные для создания файла
                    width = height = 1000;
                    bitsPerPixel = 24;
                    paletteEntries = 0;
                    palette.clear();
                    pixels.clear();

                    CreateFile = true;
                }
                if (ImGui::Button("Read File")) {
                    // Сбрасываем имя файла
                    filename = "test.ya3";
                    ReadFile = true;
                }
                ImGui::End();
            }
            

            // Если пользователь выбрал создание файла
            if (CreateFile == true) {
                ImGui::Begin("Create File");
                ImGui::InputInt("Width", &width);
                ImGui::InputInt("Height", &height);
                ImGui::InputInt("Bits Per Pixel", &bitsPerPixel);
                //ImGui::InputInt("Palette Entries", &paletteEntries);

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
                        ImGui::InputInt(("Length " + std::to_string(i + 1)).c_str(), &palette[i].length);
                        ImGui::InputInt(("R " + std::to_string(i + 1)).c_str(), &palette[i].r);
                        ImGui::InputInt(("G " + std::to_string(i + 1)).c_str(), &palette[i].g);
                        ImGui::InputInt(("B " + std::to_string(i + 1)).c_str(), &palette[i].b);
                        ImGui::InputInt(("A " + std::to_string(i + 1)).c_str(), &palette[i].a);
                    }
                }

                static float x = 0.0f, y = 0.0f;
                ImGui::InputFloat("X", &x);
                ImGui::InputFloat("Y", &y);
                if (ImGui::Button("Add")) {
                    pixels.emplace_back(x, y);
                    pixelsNotEmpty = true;
                }

                if (pixelsNotEmpty == true){
                    ImGui::Text("Pixel Coordinates:");
                    for (size_t i = 0; i < pixels.size(); ++i) {
                        ImGui::Text("Pixel %d: (%.2f, %.2f)", i + 1, pixels[i].x, pixels[i].y);
                    }
                }

                

                if (ImGui::Button("Done!")) {
                    // Запись в файл
                    std::ofstream file("test.ya3", std::ios::binary);
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

                        for (const auto& pixel : pixels) {
                            writeNumber(file, pixel.x);
                            writeNumber(file, pixel.y);
                        }

                        file.close();
                        
                        pixelShapes = loadFile(filename, imageWidth, imageHeight);
                        renderFile = true;  // Включаем рендеринг
                    }
                    //ImGui::End();
                }
                ImGui::End();
            }

            // Если пользователь выбрал чтение файла
            if (ReadFile) {
                ImGui::Begin("Read File");
                //ImGui::InputText("Filename", &filename);
                ImGui::InputText("File Name: ", &filename[0], filename.size());
                if (ImGui::Button("Done!")) {
                    pixelShapes = loadFile(filename, imageWidth, imageHeight);
                    renderFile = true;  // Включаем рендеринг
                }
                ImGui::End();
            }
        }

        ImGui::SFML::Render(window);
        //ImGui::Render();
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
