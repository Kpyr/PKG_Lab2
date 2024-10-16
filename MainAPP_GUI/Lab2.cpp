﻿#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <cmath>

// Функция для записи числа
template<typename T>
void writeNumber(std::ofstream& file, T value) {
    char* ptr = reinterpret_cast<char*>(&value);
    for (size_t i = 0; i < sizeof(T); ++i) {
        file.write(ptr + i, 1);
    }
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

struct PaletteEntry {
    int angle;
    int length;
    int r, g, b, a;
};

// Функция для загрузки пикселей из файла
std::vector<sf::CircleShape> loadFile(const std::string& filename, uint16_t& width, uint16_t& height) {
    std::ifstream file(filename, std::ios::binary);
    std::vector<sf::CircleShape> pixelShapes;

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
	int tmpScale = 1;
    while (file) {
        float x = readNumber<float>(file);
        float y = readNumber<float>(file);
		int scale = readNumber<uint16_t>(file);
        if (file) {
            pixels.emplace_back(x, y);
			scales.emplace_back(scale);
        }
    }
    file.close();

	tmpScale = scales[0];

    // Создание фигур для пикселей
    sf::Vector2f center(std::min(width, height) / 2.0f, std::min(width, height) / 2.0f);
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

            if (angle >= totalAngle && angle < totalAngle + sectorAngle + 1e-5f) {
                sectorIndex = j;
                break;
            }
            totalAngle += sectorAngle;

        }

        float radius = std::min(width, height) / 2.0f;

        if (sectorIndex != -1 && distance <= radius){
            sf::CircleShape shape(2.0f * tmpScale);
            shape.setPosition(pos);
            sf::Color color(rValues[sectorIndex], gValues[sectorIndex], bValues[sectorIndex], aValues[sectorIndex]);
            //sf::Color color(255,0,0,255);
            shape.setFillColor(color);  // Тестовый цвет
            pixelShapes.push_back(shape);
        }
    }

    return pixelShapes;
}

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

    // Чтение пикселей
    std::vector<sf::Vector2f> pixels;
    while (inputFile) {
        float x = readNumber<float>(inputFile);
        float y = readNumber<float>(inputFile);
        uint16_t scale = readNumber<uint16_t>(inputFile);
        if (inputFile) pixels.emplace_back(x, y);
    }
    inputFile.close();

    // Изменение размеров изображения
    uint16_t newWidth = width * scaleFactor;
    uint16_t newHeight = height * scaleFactor;
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
		writeNumber(outputFile, (uint16_t)scaleFactor);
    }

    outputFile.close();
}

//
//int main() {
//
//    scaleImage("test.ya3", "stest.ya3", 2);
//
//
//    return 0;
//}

//Основной код UI и логики


int main() {
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Lab2_GUI", sf::Style::Default);
    ImGui::SFML::Init(window);

    std::string filename = "";
    std::string TmpFilename = "";
    std::string inputFilename = "";
    std::string outputFilename = "";

	int scale = 1;

    bool renderFile = false;  // Флаг для рендеринга

    bool CreateFile = false;
    bool ReadFile = false;
    bool ScaleFile = false;

    bool isPalletSet = false;

    bool pixelsNotEmpty = false;

    bool isValidExtension = true;

    bool isValidInputName = true;
    bool isValidOutputName = true;

    bool isValidName = true;

    std::vector<sf::CircleShape> pixelShapes;
    uint16_t imageWidth = 0, imageHeight = 0;

    // Данные для создания файла
    //uint16_t width = 1000, height = 1000, bitsPerPixel = 24, paletteEntries = 0;
    int width = 1000, height = 1000, bitsPerPixel = 24, paletteEntries = 0;
    std::vector<PaletteEntry> palette;
    std::vector<sf::Vector2f> pixels;
	std::vector<int> scales;
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
            sf::CircleShape circle(std::min(imageWidth, imageHeight) / 2.0f);
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
            if (!CreateFile && !ReadFile && !ScaleFile) {
                ImGui::Begin("Main Menu");
                if (ImGui::Button("Create File")) {
                    // Сбрасываем данные для создания файла
                    width = height = 1000;
                    bitsPerPixel = 24;
                    paletteEntries = 0;
                    outputFilename = "example.ya3";
                    palette.clear();
                    pixels.clear();

                    CreateFile = true;
                }
                if (ImGui::Button("Read File")) {
                    // Сбрасываем имя файла
                    filename = "example.ya3";
                    ReadFile = true;
                }
                if (ImGui::Button("Scale File")) {
					outputFilename = "example.ya3";
					inputFilename = "example.ya3";
                    scale = 1;
                    ScaleFile = true;
                }
                ImGui::End();
            }
            

            // Если пользователь выбрал создание файла
            if (CreateFile == true) {
                ImGui::Begin("Create File");
                ImGui::InputText("Output File Name", &outputFilename[0], outputFilename.size() + 1);
                if (!isValidOutputName) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a valid filename with '.ya3' extension.");
                }
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
				int scale = 1;
                ImGui::InputFloat("X", &x);
                ImGui::InputFloat("Y", &y);
                ImGui::InputInt("Scale", &scale);
                if (ImGui::Button("Add")) {
                    pixels.emplace_back(x, y);
					scales.push_back(scale);
                    pixelsNotEmpty = true;
                }

                if (pixelsNotEmpty == true){
                    ImGui::Text("Pixel Coordinates:");
                    for (size_t i = 0; i < pixels.size(); ++i) {
                        ImGui::Text("Pixel %d: (%.2f, %.2f) %d", i + 1, pixels[i].x, pixels[i].y, scales[i]);
                    }
                }

                

                if (ImGui::Button("Done!")) {

                    std::string tmpString2;
                    //bool fl = true;
                    for (int i = 0; i < outputFilename.length() - 4; i++) {

                        if (outputFilename[i] == '.' && outputFilename[i + 1] == 'y' && outputFilename[i + 2] == 'a' && outputFilename[i + 3] == '3') {

                            tmpString2.append(".ya3");
                            break;
                        }
                        tmpString2.push_back(outputFilename[i]);
                    }

                    isValidOutputName = tmpString2.length() >= 5 && tmpString2.substr(tmpString2.length() - 4) == ".ya3";

                    if (isValidOutputName){
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

                            /*for (const auto& pixel : pixels) {
                                writeNumber(file, pixel.x);
                                writeNumber(file, pixel.y);
                            
                            }*/

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
                    //ImGui::End();
                }
                ImGui::End();
            }

            if (ScaleFile) {
                ImGui::Begin("Scale File");
                ImGui::InputText("Input File Name", &inputFilename[0], inputFilename.size() + 1);
				if (!isValidInputName){
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a valid filename with '.ya3' extension.");
                }
                ImGui::InputText("Output File Name", &outputFilename[0], outputFilename.size() + 1);
                if (!isValidOutputName) {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Please enter a valid filename with '.ya3' extension.");
                }
				ImGui::InputInt("Scale Factor", &scale);
                if (ImGui::Button("Done!")) {

                    std::string tmpString;
                    //bool fl = true;
                    for (int i = 0; i < inputFilename.length() - 4; i++) {

                        if (inputFilename[i] == '.' && inputFilename[i + 1] == 'y' && inputFilename[i + 2] == 'a' && inputFilename[i + 3] == '3') {

                            tmpString.append(".ya3");
                            break;
                        }
                        tmpString.push_back(inputFilename[i]);
                    }

                    isValidInputName = tmpString.length() >= 5 && tmpString.substr(tmpString.length() - 4) == ".ya3";



                    std::string tmpString2;
                    //bool fl = true;
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

            // Если пользователь выбрал чтение файла
            if (ReadFile) {
                ImGui::Begin("Read File");
                ImGui::InputText("File Name", &filename[0], filename.size()+1);
                
                if (ImGui::Button("Done!")) {
					std::string tmpString;
                    bool fl = true;
                    for (int i = 0; i < filename.length()-4; i++) {

                        if(filename[i] == '.' && filename[i+1] == 'y'&& filename[i+2] == 'a'&& filename[i+3] == '3'){

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
