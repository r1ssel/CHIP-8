#include "chip8.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


unsigned char chip8_fontset[80] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

chip8::chip8()
{
	// empty
}

chip8::~chip8()
{
	// empty
}

void chip8::init()
{
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    for (uint16_t i = 0; i < 2048; i++){
        gfx[i] = 0;
    }

    for (uint16_t i = 0; i < 16; ++i){
        stack[i] = 0;
    }

    for (uint16_t i = 0; i < 16; i++)
    {
        key[i] = V[i] = 0;
    }

    // Clear memory
	for(int i = 0; i < 4096; ++i)
		memory[i] = 0;
					
	// Load fontset
	for(int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];		

	// Reset timers
	delay_timer = 0;
	sound_timer = 0;

	// Clear screen once
	drawFlag = true;

    srand (time(NULL));
}

void chip8::emulateCycle()
{
    opcode = memory[pc] << 8 | memory[pc +1];

    printf("opcode: %X\r\n", opcode);

    switch (opcode & 0xF000)
    {
        case 0x0000:
        {
            printf("opcode & 0xF000: %X\r\n", opcode);
            switch (opcode & 0x000F)    
            {
                case 0x0000:
                    for (uint16_t i = 0; i < 2048; i++){
                        gfx[i] = 0x0;
                    }
                    drawFlag = true;
                    pc += 2;
                break;

                case 0x000E:
                    
                break;
                
                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }    
            break;
        }
        case 0xA000:
        {
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        }
        

        case 0x2000:
        {
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
        break;
        }    
        

        case 0x0004:
        {
            if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) {
                V[0xF] = 1;
            } else {
                V[0xF] = 0;
            }
            V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
            pc += 2;
        break;
        }

        case 0x0033:
        {
            memory[I]     = V[(opcode & 0x0F00) >> 8] / 100;
            memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
            memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
            pc += 2;  
        break;
        }

        case 0xD000:
        {
            uint16_t x = V[(opcode & 0x0F00) >> 8];
            uint16_t y = V[(opcode & 0x00F0) >> 4];
            uint16_t height = opcode & 0x000F;
            uint16_t pixel = 0;

            V[0xF] = 0;
            for (uint16_t yline = 0; yline < height; yline++) {
                pixel = memory[I + yline];
                for (uint16_t xline = 0; xline < 8; xline++) {
                    if ((pixel & (0x80 >> xline) != 0)) {
                        if (gfx[(x + xline + ((y + yline) * 64))] == 1) {
                            V[0xF] = 1;
                        }
                        gfx[x + xline + ((y+ yline) * 64)] ^= 1;
                    }
                }
            }

            drawFlag = true;
            pc += 2;
        break;
        }
        

        case 0xE000:
        {
            switch (opcode & 0x00FF)
            {
                case 0x009E:
                    if (key[V[(opcode & 0x0F00) >> 8]] != 0){
                        pc += 4;
                    } else {
                        pc += 2;
                    }
                break;
                
                default:
                    printf("Unknown opcode: 0x%X\n", opcode);
            }
            break;
        }

        default:
            printf("Unknown opcode: 0x%X\n", opcode);
            break;
    }

    if (delay_timer > 0) {
        --delay_timer;
    }

    if (sound_timer > 0) {
        if (sound_timer > 0) {
            printf("BEEP!");
        }
        --sound_timer;
    }

}
