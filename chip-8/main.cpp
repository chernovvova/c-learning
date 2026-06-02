#include <iostream>
#include <filesystem>

#include "Chip.h"

[[noreturn]] int main() {
   const std::filesystem::path rom_path = "resources/test/8-scrolling.ch8";

   Chip chip8{};
   chip8.readROM(rom_path);

   for (int i = 0; i < 4000; i++) {
      chip8.cycle();
   }

   chip8.print_screen();
}
