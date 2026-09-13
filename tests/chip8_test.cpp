#include <gtest/gtest.h>
#include "chip8.h"

// ============================================================
// 1NNN — Jump to address NNN
// ============================================================
TEST(Chip8, 1NNN_JumpsToAddress) {
    chip8 cpu;
    cpu.init();

    // Инструкция 0x1234: pc = 0x234
    cpu.setMemory(0x200, 0x12);
    cpu.setMemory(0x201, 0x34);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x234);
}

// ============================================================
// 2NNN — Call subroutine at NNN
// ============================================================
TEST(Chip8, 2NNN_CallsSubroutine) {
    chip8 cpu;
    cpu.init();

    // Инструкция 0x2400: push pc, pc = 0x400
    cpu.setMemory(0x200, 0x24);
    cpu.setMemory(0x201, 0x00);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x400);        // прыгнули
    EXPECT_EQ(cpu.getSp(), 1);            // стек вырос
    EXPECT_EQ(cpu.getStack(0), 0x200);    // старый pc сохранён
}

// ============================================================
// 3XNN — Skip next if VX == NN
// ============================================================
TEST(Chip8, 3XNN_SkipsWhenEqual) {
    chip8 cpu;
    cpu.init();

    // 0x3542: if V[5] == 0x42, skip next
    cpu.setV(0x5, 0x42);
    cpu.setMemory(0x200, 0x35);
    cpu.setMemory(0x201, 0x42);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x204);   // пропустили
}

TEST(Chip8, 3XNN_DoesNotSkipWhenNotEqual) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x00);   // не равно 0x42
    cpu.setMemory(0x200, 0x35);
    cpu.setMemory(0x201, 0x42);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x202);   // не пропустили
}

// ============================================================
// 4XNN — Skip next if VX != NN
// ============================================================
TEST(Chip8, 4XNN_SkipsWhenNotEqual) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x00);
    cpu.setMemory(0x200, 0x45);
    cpu.setMemory(0x201, 0x42);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x204);
}

TEST(Chip8, 4XNN_DoesNotSkipWhenEqual) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x42);
    cpu.setMemory(0x200, 0x45);
    cpu.setMemory(0x201, 0x42);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// 5XY0 — Skip next if VX == VY
// ============================================================
TEST(Chip8, 5XY0_SkipsWhenRegistersEqual) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x42);
    cpu.setV(0x6, 0x42);
    cpu.setMemory(0x200, 0x55);
    cpu.setMemory(0x201, 0x60);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x204);
}

// ============================================================
// 6XNN — Set VX = NN
// ============================================================
TEST(Chip8, 6XNN_SetsRegister) {
    chip8 cpu;
    cpu.init();

    // 0x6A42: V[0xA] = 0x42
    cpu.setMemory(0x200, 0x6A);
    cpu.setMemory(0x201, 0x42);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0xA), 0x42);
    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// 7XNN — Add NN to VX (no carry)
// ============================================================
TEST(Chip8, 7XNN_AddsNN) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x10);
    cpu.setMemory(0x200, 0x75);
    cpu.setMemory(0x201, 0x05);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x5), 0x15);
    EXPECT_EQ(cpu.getV(0xF), 0);   // 7XNN не трогает VF
}

TEST(Chip8, 7XNN_WrapsAround) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0xFF);
    cpu.setMemory(0x200, 0x75);
    cpu.setMemory(0x201, 0x02);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x5), 0x01);   // 0xFF + 0x02 = 0x101 → 0x01
    EXPECT_EQ(cpu.getV(0xF), 0);
}

// ============================================================
// 8XY0 — Set VX = VY
// ============================================================
TEST(Chip8, 8XY0_CopiesRegister) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x6, 0x42);
    cpu.setMemory(0x200, 0x85);
    cpu.setMemory(0x201, 0x60);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x5), 0x42);
}

// ============================================================
// 8XY1 — VX |= VY
// ============================================================
TEST(Chip8, 8XY1_Or) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0b1010);
    cpu.setV(0x6, 0b0110);
    cpu.setMemory(0x200, 0x85);
    cpu.setMemory(0x201, 0x61);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x5), 0b1110);
}

// ============================================================
// 8XY2 — VX &= VY
// ============================================================
TEST(Chip8, 8XY2_And) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0b1010);
    cpu.setV(0x6, 0b0110);
    cpu.setMemory(0x200, 0x85);
    cpu.setMemory(0x201, 0x62);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x5), 0b0010);
}

// ============================================================
// 8XY3 — VX ^= VY
// ============================================================
TEST(Chip8, 8XY3_Xor) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0b1010);
    cpu.setV(0x6, 0b0110);
    cpu.setMemory(0x200, 0x85);
    cpu.setMemory(0x201, 0x63);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x5), 0b1100);
}

// ============================================================
// 8XY4 — VX += VY, VF = carry
// ============================================================
TEST(Chip8, 8XY4_AddNoCarry) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x10);
    cpu.setV(0x6, 0x20);
    cpu.setMemory(0x200, 0x85);
    cpu.setMemory(0x201, 0x64);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x5), 0x30);
    EXPECT_EQ(cpu.getV(0xF), 0);
}

TEST(Chip8, 8XY4_AddWithCarry) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0xFF);
    cpu.setV(0x6, 0x02);
    cpu.setMemory(0x200, 0x85);
    cpu.setMemory(0x201, 0x64);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x5), 0x01);
    EXPECT_EQ(cpu.getV(0xF), 1);
}

// ============================================================
// 8XY5 — VX -= VY, VF = no borrow
// ============================================================
TEST(Chip8, 8XY5_SubNoBorrow) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x30);
    cpu.setV(0x6, 0x10);
    cpu.setMemory(0x200, 0x85);
    cpu.setMemory(0x201, 0x65);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x5), 0x20);
    EXPECT_EQ(cpu.getV(0xF), 1);
}

TEST(Chip8, 8XY5_SubWithBorrow) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x10);
    cpu.setV(0x6, 0x30);
    cpu.setMemory(0x200, 0x85);
    cpu.setMemory(0x201, 0x65);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x5), 0xE0);
    EXPECT_EQ(cpu.getV(0xF), 0);
}

// ============================================================
// ANNN — Set I = NNN
// ============================================================
TEST(Chip8, ANNN_SetsI) {
    chip8 cpu;
    cpu.init();

    cpu.setMemory(0x200, 0xA2);
    cpu.setMemory(0x201, 0x34);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getI(), 0x234);
    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// FX33 — BCD of VX into memory at I
// ============================================================
TEST(Chip8, FX33_StoresBCD) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 123);   // 123 = 0x7B
    cpu.setI(0x300);
    cpu.setMemory(0x200, 0xF5);
    cpu.setMemory(0x201, 0x33);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getMemory(0x300), 1);   // сотни
    EXPECT_EQ(cpu.getMemory(0x301), 2);   // десятки
    EXPECT_EQ(cpu.getMemory(0x302), 3);   // единицы
}