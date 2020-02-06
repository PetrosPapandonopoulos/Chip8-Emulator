//
// Created by Petros on 2/6/2020.
//

#include <algorithm>
#include <fstream>
#include "Chip8.h"

void Chip8::initialize() {
    unsigned char chip8Fontset[80] =
            {
                    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                    0x20, 0x60, 0x20, 0x20, 0x70, // 1
                    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
            };

    pc = 0x200;      // Program counter starts at 0x200
    opcode = 0;      // Initialize current opcode
    I      = 0;      // Initialize index register
    sp     = 0;      // Initialize stack pointer
    delayTimer = 0;  // Initialize delayTimer
    soundTimer = 0;  // Initialize soundTimer

    std::fill_n(gfx, 64 * 32, 0);
    std::fill_n(stack, 16, 0);
    std::fill_n(V, 16, 0);
    std::fill_n(memory, 4096, 0);

    // Load fontset
    for(int i = 0; i < 80; ++i) {
        memory[i] = chip8Fontset[i];
    }
}

void Chip8::loadGame(const char *romPath) {
    int fsize;

    std::ifstream rom(romPath, std::ios::binary | std::ios::ate);
    if(rom.fail()){
        throw std::runtime_error("Error: file");
    }

    fsize = rom.tellg();
    rom.seekg(0, std::ios::beg);

    rom.read(reinterpret_cast<char *>(memory + 0x200), fsize);

    rom.close();
}