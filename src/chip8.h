
#include <stdint.h>

class chip8 {
    public:
        chip8();
        ~chip8();

        bool drawFlag;

        void emulateCycle();
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

        
};