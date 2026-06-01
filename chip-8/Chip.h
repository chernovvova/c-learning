//
// Created by vovac on 31.05.2026.
//
#ifndef CHIP_8_CHIP_H
#define CHIP_8_CHIP_H

#include <cstdint>
#include <array>
#include <string>
#include <filesystem>

class Chip {
    struct OpcodeNibbles {
        uint16_t type;
        uint16_t x;
        uint16_t y;
        uint16_t n;
        uint16_t nn;
        uint16_t nnn;
        OpcodeNibbles() = default;
        OpcodeNibbles(int type, int x, int y, int n, int nn, int nnn);
    };

    std::array<uint8_t, 4096> memory {};
    std::array<uint8_t, 16> V {};
    uint16_t I = 0;
    uint16_t pc = 0x200;
    std::array<uint16_t, 16> stack {};
    uint8_t sp = 0;
    uint8_t delay = 0;
    uint8_t sound = 0;
    std::array<std::array<bool, 32>, 64> screen {};
    std::array<bool, 16> keyboard {};

    uint16_t fetch();
    OpcodeNibbles decode(uint16_t opcode);
    void execute(OpcodeNibbles nibbles);
public:
    Chip();
    void reset();
    void readROM(const std::filesystem::path& path);
    void cycle();
    void print_screen();
};


#endif //CHIP_8_CHIP_H