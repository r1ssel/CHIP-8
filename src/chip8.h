
#include <stdint.h>

class chip8 {
    public:
        chip8();
        ~chip8();

        bool drawFlag;

        void emulateCycle();
        void run();

        void debugRender();
        bool loadApplication(const char * filename);

        uint8_t gfx[64 * 32];
        uint8_t key[16];

        void init();
        
        // tests api
        uint16_t getPc()     const { return pc; }
        uint16_t getI()      const { return I; }
        uint16_t getSp()     const { return sp; }
        uint16_t getOpcode() const { return opcode; }

        uint8_t  getV(uint8_t i)     const { return V[i]; }
        uint16_t  getStack(uint8_t i) const { return stack[i]; }
        uint8_t  getMemory(uint16_t addr) const { return memory[addr]; }
        uint8_t  getDelayTimer()     const { return delay_timer; }
        uint8_t  getSoundTimer()     const { return sound_timer; }

        // --- Сеттеры для тестов (подготовка состояния) ---
        void setPc(uint16_t value)     { pc = value; }
        void setI(uint16_t value)      { I = value; }
        void setV(uint8_t i, uint8_t value)     { V[i] = value; }
        void setMemory(uint16_t addr, uint8_t value) { memory[addr] = value; }

        // --- Утилита для тестов: выполнить одну инструкцию ---
        // (можно вызывать emulateCycle напрямую, но так понятнее)
        void runInstruction() { emulateCycle(); }

    private:
        uint16_t pc;
        uint16_t opcode;
        uint16_t I;
        uint16_t sp;

        uint8_t V[16];
        uint16_t stack[16];
        uint8_t memory[4096];

        uint8_t delay_timer;
        uint8_t sound_timer;

        bool waitingForKey = false;
        uint8_t waitingKeyIndex = 0;

        void (chip8::*Chip8Table[16])();        // смотрит по старшему байту
        
        void (chip8::*Chip8Arithmetic[16 ])();    // по 0x000F для 0x8
        void (chip8::*Chip8System    [256])();    // по 0x00FF для 0x0
        void (chip8::*Chip8Keyboard  [256])();    // по 0x00FF для 0xE
        void (chip8::*Chip8Misc      [256])();    // по 0x00FF для 0xF

        void cpuNULL();

        // Simple in Chip8Table
        void cpu1NNN();   // 0x1: jump
        void cpu2NNN();   // 0x2: call
        void cpu3XNN();   // 0x3: skip if VX == NN
        void cpu4XNN();   // 0x4: skip if VX != NN
        void cpu5XY0();   // 0x5: skip if VX == VY
        void cpu6XNN();   // 0x6: VX = NN
        void cpu7XNN();   // 0x7: VX += NN
        void cpu9XY0();   // 0x9: skip if VX != VY
        void cpuANNN();   // 0xA: I = NNN
        void cpuBNNN();   // 0xB: jump to NNN + V0
        void cpuCXNN();   // 0xC: VX = rand() & NN
        void cpuDXYN();   // 0xD: draw sprite

        // Chip8System
        void cpu0NNN();  
        void cpu00E0();
        void cpu00EE();
        
        // Chip8Arithmetic
        void cpu8XYN(); 
        void cpu8XY0(); 
        void cpu8XY1();
        void cpu8XY2();
        void cpu8XY3();
        void cpu8XY4();
        void cpu8XY5();
        void cpu8XY6();
        void cpu8XY7();
        void cpu8XYE();

        // Chip8Keyboard
        void cpuEXNN(); 
        void cpuEX9E();
        void cpuEXA1();

        // Chip8Misc
        void cpuFXNN(); 
        void cpuFX07();
        void cpuFX0A();
        void cpuFX15();
        void cpuFX18();
        void cpuFX1E();
        void cpuFX29();
        void cpuFX33();
        void cpuFX55();
        void cpuFX65();
};