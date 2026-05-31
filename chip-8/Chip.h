//
// Created by vovac on 31.05.2026.
//
#pragma once

#ifndef CHIP_8_CHIP_H
#define CHIP_8_CHIP_H

#include <cstdint>
#include <array>

class Chip {
    std::array<uint8_t, 4096> memory {};
    std::array<uint8_t, 16> V {};
    uint16_t I =0;
    uint16_t pc = 0;
    std::array<uint16_t, 16> stack {};
    uint8_t sp = 0;
    uint8_t delay = 0;
    uint8_t sound = 0;
    std::array<std::array<bool, 32>, 64> screen {};
    std::array<bool, 16> keyboard {};
public:
    void reset();
};


#endif //CHIP_8_CHIP_H