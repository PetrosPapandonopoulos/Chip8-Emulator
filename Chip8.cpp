//
// Created by Petros on 2/6/2020.
//

#include <algorithm>
#include <fstream>
#include <time.h>
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

void Chip8::emulateCycle() {
    srand(time(NULL));
    //Fetch opcode
    opcode = memory[pc] << 8 | memory[pc + 1];

    //Decode opcode
    switch(opcode & 0xF000){
        case 0x0000: {

            break;
        }
        case 0x1000: {
            pc = opcode & 0x0FFF;
            break;
        }
        case 0x2000: {
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;
        }
        case 0x3000: {
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
                pc += 2;
            }
            break;
        }
        case 0x4000: {
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
                pc += 2;
            }
            break;
        }
        case 0x5000: {
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]){
                pc += 2;
            }
            break;
        }
        case 0x6000: {
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            break;
        }
        case 0x7000: {
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            break;
        }
        case 0x8000: {
            break;
        }
        case 0x9000: {
            if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]){
                pc += 2;
            }

            break;
        }
        case 0xA000: {                      // ANNN: Sets I to the address NNN
            I = opcode & 0x0FFF;            // use mask 0x0FFF in order to take the address
            pc += 2;
            break;
        }
        case 0xB000: {                      //BNNN: Jumps to the address NNN plus V0
            pc = V[0] + opcode & 0x0FFF;
            break;
        }
        case 0xC000: {           //CXNN: Sets V[X] to the result of a & operation on a random number (0 to 255) and NN
            V[opcode & 0x0F00] = (rand() % 256) & (opcode & 0x00FF);
            break;
        }
        case 0xD000: {
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short pixel;

            V[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
                pixel = memory[I + yline];
                for(int xline = 0; xline < 8; xline++)
                {
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(gfx[(x + xline + ((y + yline) * 64))] == 1)
                            V[0xF] = 1;
                        gfx[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }

            drawFlag = true;
            pc += 2;
            break;
        }
        case 0xE000: {

            break;
        }
        case 0xF000: {

            break;
        }
    }


}
