//
// Created by vovac on 31.05.2026.
//

#include "Chip.h"

#include <cassert>
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

Chip::OpcodeNibbles::OpcodeNibbles(const int type, const int x, const int y, const int n, const int nn, const int nnn) {
    this->type = type;
    this->x = x;
    this->y = y;
    this->n = n;
    this->nn = nn;
    this->nnn = nnn;
}

void Chip::reset() {
    memory.fill(0);
    V.fill(0);
    I = 0;
    pc = 0x200;
    stack.fill(0);
    sp = 0;
    delay = 0;
    sound = 0;
    screen.fill({});
    keyboard.fill(false);
}

void Chip::print_screen() {
    for (int j = 0; j < screen[0].size(); j++) {
        for (int i = 0; i < screen.size(); i++) {
            if (screen[i][j]) {
                std::cout << "#";
            }
            else {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}

void Chip::readROM(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
        return;
    }

    file.seekg(0, std::ios::end);
    uint16_t file_size = file.tellg();
    if (file_size > (4096 - 0x200)) {
        std::cerr << "File to large. Maximum file size " << 4096 - 0x200 << std::endl;
        return;
    }
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&memory[0x200]), file_size);
}

void Chip::cycle() {
    const uint16_t opcode = fetch();
    const OpcodeNibbles nibbles = decode(opcode);
    execute(nibbles);
}

uint16_t Chip::fetch() {
    assert(pc <= memory.size());
    const uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;
    return opcode;
}

Chip::OpcodeNibbles Chip::decode(const uint16_t opcode) {
    return {opcode & 0xF000, (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4, opcode & 0x000F, opcode & 0x00FF, opcode & 0x0FFF};
}

void Chip::execute(const OpcodeNibbles nibbles) {
    switch (nibbles.type) {
        case 0: {
            switch (nibbles.nn) {
                case 0xE0: {
                    screen.fill({});
                    break;
                }
                default: {
                    std::cerr << "Unknown opcode" << std::endl;
                }
            }
            break;
        }
        case 0x1000: {
            pc = nibbles.nnn;
            break;
        }
        case 0x6000: {
            V[nibbles.x] = nibbles.nn;
            break;
        }
        case 0x7000: {
            V[nibbles.x] += nibbles.nn;
            break;
        }
        case 0xA000: {
            I = nibbles.nnn;
            break;
        }
        case 0xD000: {
            uint16_t start_x = V[nibbles.x] % 64;
            uint16_t start_y = V[nibbles.y] % 32;
            V[0xF] = 0;

            for (int i = 0; i < nibbles.n; i++) {
                uint16_t current_byte = memory[I + i];
                uint16_t py = start_y + i;
                for (int j = 0; j < 8; j++) {
                    uint16_t current_bit = current_byte & (0x80 >> j);
                    if (current_bit) {
                        uint16_t px = start_x + j;
                        if (px >= 64 && py >= 32) {
                            continue;
                        }
                        if (screen[px][py]) {
                            V[0xF] = 1;
                        }
                        screen[px][py] = !screen[px][py];
                    }
                }
            }
            break;
        }
        default: {
            std::cerr << "Unknown opcode" << std::endl;
        }
    }
}