//
// Created by vovac on 31.05.2026.
//

#include "Chip.h"

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
