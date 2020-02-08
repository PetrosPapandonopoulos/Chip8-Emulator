//
// Created by Petros on 2/6/2020.
//

#ifndef CHIP8VM_CHIP8_H                 //Headerguards
#define CHIP8VM_CHIP8_H
class Chip8 {
    private:
        unsigned short opcode;			//current opcode all two bytes long
        unsigned char memory[4096];		//Chip8 memory has 4K memory
        unsigned char V[16];			//There are 16 register

        unsigned short I;				//
        unsigned short pc;				// pc counter
        unsigned char gfx[64 * 32];		//Array for graphics (holds the pixel state 1 or 0)

        unsigned short stack[16];		//Chip8 uses a stack
        unsigned short sp;				//stack pointer

        bool keys[16];			//HEX based keypad (0x0-0xF)

        unsigned char delayTimer;		//The two timer registers that count at 60 Hz
        unsigned char soundTimer;		//The system’s buzzer sounds whenever the sound timer reaches zero
        bool drawFlag;
        bool isRunning;
        bool waitForKey;
           
    public:
        Chip8();
        bool emulateCycle(int cycle);
        void loadGame(const char* romPath);
        bool DecodeOpcode();
        void keyPressed(char key);
        void keyReleased(char key);
        void playSound();
        unsigned char getPixel(int i);


};
#endif //CHIP8VM_CHIP8_H
