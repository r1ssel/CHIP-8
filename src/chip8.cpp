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
    printf("init() called\n");
    pc = 0x200;
    printf("init: pc=0x%03X\n", pc);
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
    // printf("pc=0x%03X opcode=0x%04X\n", pc, opcode);
    if (opcode == 0xF090) exit(0);
    // printf("opcode: %X\r\n", opcode);

    switch (opcode & 0xF000)
    {
        case 0x0000:
        {
            switch (opcode & 0x000F)    
            {
                case 0x0000:
                {
                    for (uint16_t i = 0; i < 2048; i++){
                        gfx[i] = 0x0;
                    }
                    drawFlag = true;
                    pc += 2;
                    break;
                }

                case 0x000E:
                {
                    --sp;
                    pc = stack[sp];
                    pc += 2;
                    break;
                }
                
                
                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
                    exit(0);
            }    
            break;
        }
        
        case 0x1000: // 0x1NNN: Jumps to address NNN
		{	
            pc = opcode & 0x0FFF;
		    break;
        }

		case 0x2000: // 0x2NNN: Calls subroutine at NNN.
		{
            stack[sp] = pc;			// Store current address in stack
            ++sp;					// Increment stack pointer
            pc = opcode & 0x0FFF;	// Set the program counter to the address at NNN
            break;
        }

        case 0x3000:
        {
            if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;
        }
        
        case 0x4000:
        {
            if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;
        }

        case 0x5000:
        {
            if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;
        }

        case 0x6000:
        {
            V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
            pc += 2;
            break;
        }

        case 0x7000:
        {
            V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
            pc += 2;
            break;
        }

        case 0x8000:
        {
            switch(opcode & 0x000F)
            {
                case 0x0000:
                {
                    V[(opcode & 0x00F0) >> 4] = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                }

                case 0x0001:
                {
                    V[(opcode & 0x00F0) >> 4] |= V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                }

                case 0x0002:
                {
                    V[(opcode & 0x00F0) >> 4] &= V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                }

                case 0x0003:
                {
                    V[(opcode & 0x00F0) >> 4] ^= V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                }

                case 0x0004:
                {
                    if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];  // VX += VY
                    pc += 2;
                    break;
                }

                case 0x0005:
                {
                    if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) {
                        V[0xF] = 0;
                    } else {
                        V[0xF] = 1;
                    }
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                }

                case 0x0006:
                {
                    // in example order is different
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;  // бит ДО сдвига
                    V[(opcode & 0x0F00) >> 8] >>= 1;
                    pc += 2;
                    break;
                }

                case 0x0007:
                {
                    if (V[(opcode & 0x00F0) >> 4] >= V[(opcode & 0x0F00) >> 8]) {
                        V[0xF] = 1;
                    } else {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8]);
                    pc += 2;
                    break;
                }

                case 0x000E:
                {
                    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;
                    break;
                }

                default:
                {
                    printf("im not going to proccess this\r\n");
                    // break;
                }
            }
            break;
        }

        case 0x9000:
        {
            if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
                pc += 4;
            } else {
                pc += 2;
            }
            break;
        }

        case 0xA000:
        {
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        }
        
        case 0xB000:
        {
            pc = V[0] + (opcode & 0x0FFF);
            break;
        }

        case 0xC000:
        {
            V[(opcode & 0x0F00) >> 8] = (rand() % 0x100) & (opcode & 0x00FF);
            pc += 2;
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
                    if ((pixel & (0x80 >> xline)) != 0) {
                        uint16_t px = (x + xline) % 64;
                        uint16_t py = (y + yline) % 32;
                        uint16_t idx = px + py * 64;
                        if (gfx[idx] == 1) V[0xF] = 1;
                        gfx[idx] ^= 1;
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

                case 0x00A1:
                {
                    if (key[V[(opcode & 0x0F00) >> 8]]) {
                        pc += 4;
                    } else {
                        pc += 2;
                    }
                    break;
                }
                
                default:
                    printf("Unknown opcode: 0x%X\n", opcode);
                    exit(0);
                  
            }
            break;
        }

        case 0xF000:
        {
            switch (opcode & 0x00FF)
            {
                case 0x0007:
                {
                    V[(opcode & 0x0F00) >> 8] = delay_timer;
                    pc += 2;
                    break;
                }

                case 0x000A:
                {
                    bool keyPress = false;

                    for (uint16_t i = 0; i < 16; i++) {
                        if (key[i] != 0)
                        {
                            V[(opcode & 0x0F00) >> 8] = i;
							keyPress = true;
                        }
                    }

                    if (!keyPress) {
                        return;
                    }

                    pc += 2;
                    break;
                }

                case 0x0015:
                {
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                }

                case 0x0018:
                {
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                }

                case 0x001E:
                {
                    uint8_t x = (opcode & 0x0F00) >> 8;
                    if (I + V[x] > 0xFFF)
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    I += V[x];
                    pc += 2;
                    break;
                }

                case 0x0029:
                {
                    I = V[(opcode & 0x0F00) >> 8] * 0x5;
                    pc += 2;
                    break;
                }

                case 0x0033:
                {
                    memory[I]     = (V[(opcode & 0x0F00) >> 8] / 100);
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
                    pc += 2;
                    break;
                }

                case 0x0055: // FX55: Stores V0 to VX in memory starting at address I					
				{
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                        memory[I + i] = V[i];	

                    // On the original interpreter, when the operation is done, I = I + X + 1.
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    pc += 2;
                    break;
                }

				case 0x0065: // FX65: Fills V0 to VX with values from memory starting at address I					
				{	
                    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
						V[i] = memory[I + i];			

					// On the original interpreter, when the operation is done, I = I + X + 1.
					I += ((opcode & 0x0F00) >> 8) + 1;
					pc += 2;
				    break;
                }

                default:
                {
                    printf ("Unknown opcode [0xF000]: 0x%X\n", opcode);
                    exit(0);
                }
            }
            break;
        }

        default:
        {
            printf("Unknown opcode: 0x%X.\n", opcode);
            exit(0);
            // pc += 2;
        }
    }

    if (delay_timer > 0) {
        --delay_timer;
    }

    if (sound_timer > 0) {
        if (sound_timer > 0) {
            // printf("BEEP!");
        }
        --sound_timer;
    }

}

bool chip8::loadApplication(const char * filename)
{
    init();

    printf("Loading: %s\n", filename);

    FILE * pFile = fopen(filename, "rb");
    if (pFile == NULL)
    {
        fputs ("File error", stderr);
        return false;
    }

    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    rewind(pFile);
    printf("Filesize: %d\n", (int)lSize);

    char * buffer  = (char*)malloc(sizeof(char) * lSize);
    if (buffer == NULL) 
	{
		fputs ("Memory error", stderr); 
		return false;
	}

	// Copy the file into the buffer
	size_t result = fread(buffer, 1, lSize, pFile);
	if (result != lSize) 
	{
		fputs("Reading error",stderr); 
		return false;
	}

    int inst = 32;

    

    // Copy buffer to Chip8 memory
	if((4096-512) > lSize)
	{
		for(int i = 0; i < lSize; ++i)
			memory[i + 512] = buffer[i];

        printf("Memory table (first %u instructions): \r\n", inst);

            for (int n = 0; n < inst; ++n) {
                int i = 512 + n * 2;
                printf(" [%d] = 0x%04X ", n, (memory[i] << 8) | memory[i + 1]);
                if ((n + 1) % 8 == 0) printf("\r\n");
            }
            printf("\r\n");
	}
	else
		printf("Error: ROM too big for memory");
	
    

	// Close file, free buffer
	fclose(pFile);
	free(buffer);

    printf("After load: pc=0x%03X\n", pc);
	return true;
}
