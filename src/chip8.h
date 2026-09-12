
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

        void init();
};