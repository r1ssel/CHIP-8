#include <gtest/gtest.h>
#include "chip8.h"

TEST(Chip8, Opcode6XNN_SetsRegister) {
    chip8 cpu;
    cpu.init();

    cpu.setMemory(0x200, 0x6A);
    cpu.setMemory(0x201, 0x42);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0xA), 0x42);
    EXPECT_EQ(cpu.getPc(), 0x202);
}