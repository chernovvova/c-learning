//
// Created by vovac on 31.05.2026.
//

#include "Chip.h"
#include <iostream>
#include <fstream>
#include <string>

constexpr std::array<uint8_t, 80> FONT = {
    // 0
    0b11110000,
    0b10010000,
    0b10010000,
    0b10010000,
    0b11110000,
    // 1
    0b00100000,
    0b01100000,
    0b00100000,
    0b00100000,
    0b01110000,
    // 2
    0b11110000,
    0b00010000,
    0b11110000,
    0b10000000,
    0b11110000,
    // 3
    0b11110000,
    0b00010000,
    0b11110000,
    0b00010000,
    0b11110000,
    // 4
    0b10010000,
    0b10010000,
    0b11110000,
    0b00010000,
    0b00010000,
    // 5
    0b11110000,
    0b10000000,
    0b11110000,
    0b00010000,
    0b11110000,
    // 6
    0b11110000,
    0b10000000,
    0b11110000,
    0b10010000,
    0b11110000,
    // 7
    0b11110000,
    0b00010000,
    0b00100000,
    0b01000000,
    0b01000000,
    // 8
    0b11110000,
    0b10010000,
    0b11110000,
    0b10010000,
    0b11110000,
    // 9
    0b11110000,
    0b10010000,
    0b11110000,
    0b00010000,
    0b11110000,
    // A
    0b11110000,
    0b10010000,
    0b11110000,
    0b10010000,
    0b10010000,
    // B
    0b11100000,
    0b10010000,
    0b11110000,
    0b10010000,
    0b11100000,
    // C
    0b01100000,
    0b10010000,
    0b10000000,
    0b10010000,
    0b01100000,
    // D
    0b11100000,
    0b10010000,
    0b10010000,
    0b10010000,
    0b11100000,
    // E
    0b11110000,
    0b10000000,
    0b11110000,
    0b10000000,
    0b11110000,
    // F
    0b11110000,
    0b10000000,
    0b11110000,
    0b10000000,
    0b10000000,
};
constexpr uint16_t FONT_START = 0x50;

Chip::Chip() {
    reset();
    for (int i = 0; i < FONT.size(); i++) {
        memory[FONT_START + i] = FONT[i];
    }
}

void Chip::reset() {
    memory.fill(0);
    V.fill(0);
    I = 0;
    pc = 0;
    stack.fill(0);
    sp = 0;
    delay = 0;
    sound = 0;
    screen.fill({});
    keyboard.fill(false);
}

void Chip::readROM(const std::string& path) {
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
    }

    file.seekg(0, std::ios::end);
    if (file.tellg() > (4096 - 0x200)) {
        std::cerr << "File to large. Maximum file size " << 4096 - 0x200 << std::endl;
    }
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&memory[0x200]), file.gcount());
}
