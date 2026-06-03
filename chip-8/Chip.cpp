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
    rand_generator.seed(std::random_device{}());
}

void Chip::print_screen() const {
    for (int j = 0; j < screen[0].size(); j++) {
        for (auto & i : screen) {
            if (i[j]) {
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
    const uint16_t file_size = file.tellg();
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
                case 0xEE: {
                    pc = stack[--sp];
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
        case 0x2000: {
            stack[sp++] = pc;
            pc = nibbles.nnn;
            break;
        }
        case 0x3000: {
            if (V[nibbles.x] == nibbles.nn) {
                pc += 2;
            }
            break;
        }
        case 0x4000: {
            if (V[nibbles.x] != nibbles.nn) {
                pc += 2;
            }
            break;
        }
        case 0x5000: {
            switch (nibbles.n){
                case 0x0: {
                    if (V[nibbles.x] == V[nibbles.y]) {
                        pc += 2;
                    }
                    break;
                }
                default: {
                    std::cerr << "Unknown opcode" << std::endl;
                }
            }
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
        case 0x8000: {
            switch (nibbles.n) {
                case 0x0000: {
                    V[nibbles.x] = V[nibbles.y];
                    break;
                }
                case 0x0001: {
                    V[nibbles.x] = V[nibbles.x] | V[nibbles.y];
                    break;
                }
                case 0x0002: {
                    V[nibbles.x] = V[nibbles.x] & V[nibbles.y];
                    break;
                }
                case 0x0003: {
                    V[nibbles.x] = V[nibbles.x] ^ V[nibbles.y];
                    break;
                }
                case 0x0004: {
                    uint8_t flag;
                    if (static_cast<int>(V[nibbles.x]) + static_cast<int>(V[nibbles.y]) > 255) {
                        flag = 1;
                    }
                    else {
                        flag = 0;
                    }
                    V[nibbles.x] += V[nibbles.y];
                    V[0xF] = flag;
                    break;
                }
                case 0x0005: {
                    uint8_t flag;
                    if (V[nibbles.x] >= V[nibbles.y]) {
                        flag = 1;
                    }
                    else {
                        flag = 0;
                    }
                    V[nibbles.x] -= V[nibbles.y];
                    V[0xF] = flag;
                    break;
                }
                case 0x0006: {
                    const uint8_t shifted_bit = V[nibbles.x] & 1;
                    V[nibbles.x] >>= 1;
                    V[0xF] = shifted_bit;
                }
                case 0x0007: {
                    uint8_t flag;
                    if (V[nibbles.y] >= V[nibbles.x]) {
                        flag = 1;
                    }
                    else {
                        flag = 0;
                    }
                    V[nibbles.x] -= V[nibbles.y];
                    V[0xF] = flag;
                    break;
                }
                case 0x000E: {
                    const uint8_t shifted_bit = V[nibbles.x] >> 7;
                    V[nibbles.x] <<= 1;
                    V[0xF] = shifted_bit;
                    break;
                }
                default: {
                    std::cerr << "Unknown opcode" << std::endl;
                }
            }
            break;
        }
        case 0x9000: {
            if (nibbles.n == 0 && V[nibbles.x] != V[nibbles.y]) {
                pc += 2;
            }
            break;
        }
        case 0xA000: {
            I = nibbles.nnn;
            break;
        }
        case 0xB000: {
            pc = nibbles.nnn + V[0];
            break;
        }
        case 0xC000: {
            std::uniform_int_distribution<uint8_t> distribution(0, 255);
            V[nibbles.x] = distribution(rand_generator) & nibbles.nn;
            break;
        }
        case 0xD000: {
            const uint16_t start_x = V[nibbles.x] % 64;
            const uint16_t start_y = V[nibbles.y] % 32;
            V[0xF] = 0;

            for (int i = 0; i < nibbles.n; i++) {
                const uint16_t current_byte = memory[I + i];
                const uint16_t py = start_y + i;
                for (int j = 0; j < 8; j++) {
                    if (current_byte & (0x80 >> j)) {
                        const uint16_t px = start_x + j;
                        if (px >= 64 || py >= 32) {
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
        case 0xE000: {
            switch (nibbles.nn) {
                case 0x9E: {
                    if (keyboard[V[nibbles.x]]) {
                        pc += 2;
                    }
                    break;
                }
                case 0xA1: {
                    if (!keyboard[V[nibbles.x]]) {
                        pc += 2;
                    }
                    break;
                }
                default: {
                    std::cerr << "Unknown opcode" << std::endl;
                }
                }
            break;
        }
        case 0xF000: {
            switch (nibbles.nn) {
                case 0x07: {
                    V[nibbles.x] = delay;
                    break;
                }
                case 0x0A: {
                    for (const bool i : keyboard) {
                        if (i) {
                            break;
                        }
                    }
                    pc -= 2;
                    break;
                }
                case 0x15: {
                    delay = V[nibbles.x];
                    break;
                }
                case 0x18: {
                    sound = V[nibbles.x];
                    break;
                }
                case 0x1E: {
                    I += V[nibbles.x];
                    break;
                }
                case 0x29: {
                    I = FONT_START + V[nibbles.x] * 5;
                    break;
                }
                case 0x33: {
                    memory[I] = V[nibbles.x] / 100;
                    memory[I + 1] = V[nibbles.x] / 10 % 10;
                    memory[I + 2] = V[nibbles.x] % 10;
                    break;
                }
                case 0x55: {
                    for (int i = 0; i < nibbles.x + 1; i++) {
                        memory[I + i] = V[i];
                    }
                    break;
                }
                case 0x65: {
                    for (int i = 0; i < nibbles.x + 1; i++) {
                        V[i] = memory[I + i];
                    }
                    break;
                }
                default: {
                    std::cerr << "Unknown opcode" << std::endl;
                }
            }
            break;
        }
        default: {
            std::cerr << "Unknown opcode" << std::endl;
        }
    }
}

void Chip::tick_timers() {
    if (delay > 0) {
        delay--;
    }
    if (sound > 0) {
        sound--;
    }
}