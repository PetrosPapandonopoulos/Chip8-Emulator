#include <iostream>
#include "Chip8.h"
#include <SFML/Graphics.hpp>

#define RES_MULT 20

int main(int argc, char** argv) {
    sf::RenderWindow window(sf::VideoMode(64 * RES_MULT, 32 * RES_MULT), "Chip8 Emulator",
        sf::Style::Titlebar | sf::Style::Close);

    sf::Color bgColor(sf::Color::Black);
    sf::Color fgColor(sf::Color::White);

    Chip8 chip;
    try {
        chip.loadGame(argv[1]);
    }
    catch (const std::runtime_error & error) {
        window.close();
        std::cout << "error reading rom" << std::endl;
        exit(2);
    }

 while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed: {
                switch (event.key.code) {
                case sf::Keyboard::Num1: chip.keyPressed(0x1); break;
                case sf::Keyboard::Num2: chip.keyPressed(0x2); break;
                case sf::Keyboard::Num3: chip.keyPressed(0x3); break;
                case sf::Keyboard::Num4: chip.keyPressed(0xc); break;
                case sf::Keyboard::Q: chip.keyPressed(0x4); break;
                case sf::Keyboard::W: chip.keyPressed(0x5); break;
                case sf::Keyboard::E: chip.keyPressed(0x6); break;
                case sf::Keyboard::R: chip.keyPressed(0xd); break;
                case sf::Keyboard::A: chip.keyPressed(0x7); break;
                case sf::Keyboard::S: chip.keyPressed(0x8); break;
                case sf::Keyboard::D: chip.keyPressed(0x9); break;
                case sf::Keyboard::F: chip.keyPressed(0xe); break;
                case sf::Keyboard::Z: chip.keyPressed(0xa); break;
                case sf::Keyboard::X: chip.keyPressed(0x0); break;
                case sf::Keyboard::C: chip.keyPressed(0xb); break;
                case sf::Keyboard::V: chip.keyPressed(0xf); break;
                }
                break;
            }
            case sf::Event::KeyReleased: {
                switch (event.key.code) {
                case sf::Keyboard::Num1: chip.keyReleased(0x1); break;
                case sf::Keyboard::Num2: chip.keyReleased(0x2); break;
                case sf::Keyboard::Num3: chip.keyReleased(0x3); break;
                case sf::Keyboard::Num4: chip.keyReleased(0xc); break;
                case sf::Keyboard::Q: chip.keyReleased(0x4); break;
                case sf::Keyboard::W: chip.keyReleased(0x5); break;
                case sf::Keyboard::E: chip.keyReleased(0x6); break;
                case sf::Keyboard::R: chip.keyReleased(0xd); break;
                case sf::Keyboard::A: chip.keyReleased(0x7); break;
                case sf::Keyboard::S: chip.keyReleased(0x8); break;
                case sf::Keyboard::D: chip.keyReleased(0x9); break;
                case sf::Keyboard::F: chip.keyReleased(0xe); break;
                case sf::Keyboard::Z: chip.keyReleased(0xa); break;
                case sf::Keyboard::X: chip.keyReleased(0x0); break;
                case sf::Keyboard::C: chip.keyReleased(0xb); break;
                case sf::Keyboard::V: chip.keyReleased(0xf); break;
                }
                break;
            }
            }
        }

        if (!chip.emulateCycle(1)) {
            std::cout << "Emulation failed" << std::endl;
            window.close();
            exit(3);
        }

        sf::RectangleShape pixel;
        window.clear();
        for (int i = 0; i < 64 * 32; i++) {
            pixel.setFillColor(chip.getPixel(i) ? fgColor : bgColor);
            pixel.setPosition(i % 64 * RES_MULT, i / 64 * RES_MULT);
            pixel.setSize(sf::Vector2f(RES_MULT, RES_MULT));
            window.draw(pixel);
        }

        window.display();
    }

    return 0;
}
