//
// Created by Petros on 2/6/2020.
//

#include <algorithm>
#include <fstream>
#include <ctime>
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
    isRunning = true;
    waitForKey = false;

    std::fill_n(gfx, 64 * 32, 0);
    std::fill_n(stack, 16, 0);
    std::fill_n(V, 16, 0);
    std::fill_n(memory, 4096, 0);

    // Load fontset
    for(int i = 0; i < 80; ++i) {
        memory[i] = chip8Fontset[i];
    }
    //Initialize srand (random number is needed at opcode: CXNN)
    srand(time(NULL));
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

bool Chip8::emulateCycle(){
    for(;;){
        //Fetch opcode
        opcode = memory[pc] << 8 | memory[pc+1];
        DecodeOpcode();




        if(delayTimer > 0){
            --delayTimer;
        }
        if(soundTimer > 0){
            if(soundTimer == 1){
                //sound must be heard
            }
            --soundTimer;
        }
    }


}

void Chip8::keyPressed(char key){
    keys[key] = true;
}

void Chip8::keyReleased(char key){
    keys[key] = false;
}

bool Chip8::DecodeOpcode() {
    //Decode opcode
    switch(opcode & 0xF000){
        case 0x0000:{
            switch(opcode & 0x00FF){
                case 0x00E0:{           //00E0: Clears the screen
                    std::fill_n(gfx, 64 * 32, 0);
                    pc += 2;
                    break;
                }
                case 0x00EE:{           //00EE: Returns from a subroutine
                    pc = stack[sp--];
                    break;
                }
            }
            break;
        }
        case 0x1000: {              //1NNN: Jumps to address NNN
            pc = opcode & 0x0FFF;
            break;
        }
        case 0x2000: {          //2NNN: Calls subroutine at NNN
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;
        }
        case 0x3000: {      //3XNN: Skips the next instruction if VX equals NN
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)){
                pc += 4;
            }
            else{
                pc += 2;
            }
            break;
        }
        case 0x4000: {      //4XNN: Skips the next instruction if VX doesn't equal NN
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)){
                pc += 4;
            }
            else{
                pc += 2;
            }
            break;
        }
        case 0x5000: {      //5XY0: Skips the next instruction if VX equals VY
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]){
                pc += 4;
            }
            else{
                pc += 2;
            }
            break;
        }
        case 0x6000: {      //6XNN: Sets VX to NN
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;
        }
        case 0x7000: {      //7XNN: Adds NN to VX. (Carry flag is not changed)
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;
        }
        case 0x8000: {
            switch (opcode & 0x000F){
                case 0x0000:{       //8XY0: Sets VX to the value of VY
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                }
                case 0x0001:{       //8XY1: Sets VX to VX or VY. (Bitwise OR operation)
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                }
                case 0x0002:{       //8XY2: Sets VX to VX and VY. (Bitwise AND operation)
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                }
                case 0x0003:{       //8XY3: Sets VX to VX xor VY
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                }
                case 0x0004:{       /*8XY4: Adds VY to VX.
                                     *VF is set to 1 when there's a carry, and to 0 when there isn't.*/
                    if(V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])){
                        V[0xF] = 1; //carry
                    }
                    else{
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                }
                case 0x0005:{       /*8XY5: VY is subtracted from VX.
                                     *VF is set to 0 when there's a borrow, and 1 when there isn't.*/
                    if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]){
                        V[0xF] = 1; //carry
                    }
                    else{
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                }
                case 0x0006:{       /*8XY6: Stores the least significant bit of
                                     *VX in VF and then shifts VX to the right by 1.*/
                    V[0xF] = V[(opcode & 0x0F00)] & 1;
                    V[(opcode & 0x0f00) >> 8] >>= 1;
                    pc += 2;
                    break;
                }
                case 0x0007:{       /*8XY7: Sets VX to VY minus VX.
                                     *VF is set to 0 when there's a borrow, and 1 when there isn't.*/
                    if((V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8]) >= 0x0000){
                        V[0xF] = 1; //carry
                    }
                    else{
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                }
                case 0x000E:{       /*8XYE: Stores the most significant bit of
                                     *VX in VF and then shifts VX to the left by 1*/
                    V[0xF] = V[(opcode & 0x0F00)] & 1;
                    V[(opcode & 0x0f00) >> 8] <<= 1;
                    pc += 2;
                    break;
                }
            }
            break;
        }
        case 0x9000: {          //9XY0: Skips the next instruction if VX doesn't equal VY
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
            pc += 2;
            break;
        }
        case 0xC000: {           //CXNN: Sets V[X] to the result of a & operation on a random number (0 to 255) and NN
            V[opcode & 0x0F00] = (rand() % 256) & (opcode & 0x00FF);
            pc += 2;
            break;
        }
        case 0xD000: {         /*Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and
                                *a height of N pixels. Each row of 8 pixels is read as bit-coded
                                *starting from memory location I; I value doesn’t change after the execution of this
                                *instruction. As described above, VF is set to 1 if any screen pixels are flipped from
                                *set to unset when the sprite is drawn, and to 0 if that doesn’t happen*/
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
            switch (opcode & 0x00FF){
                case 0x009E:{       //EX9E: Skips the next instruction if the key stored in VX is pressed
                    if (keys[V[(opcode & 0x0f00) >> 8]]){
                        pc += 4;
                    }
                    else{
                        pc += 2;
                    }
                    break;
                }
                case 0x00A1:{       //EXA1: Skips the next instruction if the key stored in VX isn't pressed
                    if (!keys[V[(opcode & 0x0f00) >> 8]]){
                        pc += 4;
                    }
                    else{
                        pc += 2;
                    }
                    break;
                }
            }

            break;
        }
        case 0xF000: {
            switch(opcode & 0x00FF){
                case 0x0007:{       //FX07: Sets VX to the value of the delay timer
                    V[(opcode & 0x0f00) >> 8] = delayTimer;
                    pc += 2;
                    break;
                }
                case 0x000A:{       //A key press is awaited, and then stored in VX. (Blocking Operation.
                                    // All instruction halted until next key event
                    waitForKey = true;
                    isRunning = false;
                    break;
                }
                case 0x0015:{       //FX15: Sets the delay timer to VX
                    delayTimer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                }
                case 0x0018:{       //FX18: Sets the sound timer to VX
                    soundTimer = V[(opcode & 0x0F00) >> 8];
                    break;
                }
                case 0x001E:{       /*Adds VX to I.
                                     *VF is set to 1 when there is a range
                                     *overflow (I+VX>0xFFF), and to 0 when there isn't*/

                    if(I + V[(opcode & 0x0F00) >> 8] > 0xFFF){
                        V[0xF] = 1;
                    }
                    else{
                        V[0xF] = 0;
                    }
                    I += V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                }
                case 0x0029:{       //FX29: Sets I to the location of the sprite for the character in VX
                                    //Characters 0-F (in hexadecimal) are represented by a 4x5 font.
                    I = V[(opcode & 0x0F00) >> 8] * 5;
                    pc += 2;
                    break;
                }
                case 0x0033:{
                    unsigned short X = (opcode & 0x0F00) >> 8;
                    memory[I] = V[X] / 100;
                    memory[I + 1] = (V[X] % 100) / 10;
                    memory[I + 2] = V[X] % 10;
                    pc += 2;
                    break;
                }
                case 0x0055:{
                    auto X = (opcode & 0x0F00) >> 8;
                    for(auto i = 0; i<=X ; i++){
                        memory[I+i] = V[X];
                    }
                    pc += 2;
                    break;
                }
                case 0x0065:{
                    auto X = (opcode & 0x0f00) >> 8;
                    for (auto i = 0; i <= X; i++){
                        V[i] = memory[I + i];
                    }
                    pc += 2;
                    break;
                }
            }
            break;
        }
    }
    return true;
}
