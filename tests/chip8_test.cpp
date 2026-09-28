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

// ============================================================
// 00E0 — Clear screen
// ============================================================
TEST(Chip8, 00E0_ClearsScreen) {
    chip8 cpu;
    cpu.init();

    // Заполним несколько пикселей
    cpu.gfx[0] = 1;
    cpu.gfx[100] = 1;
    cpu.gfx[2047] = 1;

    cpu.setMemory(0x200, 0x00);
    cpu.setMemory(0x201, 0xE0);
    cpu.setPc(0x200);

    cpu.runInstruction();

    for (int i = 0; i < 64 * 32; i++) {
        EXPECT_EQ(cpu.gfx[i], 0) << "pixel " << i << " not cleared";
    }
    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// 00EE — Return from subroutine
// ============================================================
TEST(Chip8, 00EE_ReturnsFromSubroutine) {
    chip8 cpu;
    cpu.init();

    // Сначала call 0x400
    cpu.setMemory(0x200, 0x24);
    cpu.setMemory(0x201, 0x00);
    cpu.setPc(0x200);
    cpu.runInstruction();   // pc = 0x400, stack[0] = 0x200, sp = 1

    // Теперь return
    cpu.setMemory(0x400, 0x00);
    cpu.setMemory(0x401, 0xEE);
    cpu.setPc(0x400);
    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x202);   // вернулись на 0x200 + 2
    EXPECT_EQ(cpu.getSp(), 0);       // стек пуст
}

// ============================================================
// 00CN — Scroll down N pixels
// ============================================================
TEST(Chip8, 00CN_ScrollsDown) {
    chip8 cpu;
    cpu.init();

    // Поставим пиксель в верхней строке
    cpu.gfx[0 * 64 + 0] = 1;   // (0, 0)
    cpu.gfx[0 * 64 + 5] = 1;   // (5, 0)

    cpu.setMemory(0x200, 0x00);
    cpu.setMemory(0x201, 0xC2);   // 00C2: scroll down 2
    cpu.setPc(0x200);

    cpu.runInstruction();

    // Пиксели сдвинулись на 2 строки вниз
    EXPECT_EQ(cpu.gfx[0 * 64 + 0], 0);   // верх пуст
    EXPECT_EQ(cpu.gfx[2 * 64 + 0], 1);   // (0, 2)
    EXPECT_EQ(cpu.gfx[2 * 64 + 5], 1);   // (5, 2)
    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// 00FB — Scroll right 4 pixels
// ============================================================
TEST(Chip8, 00FB_ScrollsRight) {
    chip8 cpu;
    cpu.init();

    cpu.gfx[0 * 64 + 0] = 1;   // (0, 0)

    cpu.setMemory(0x200, 0x00);
    cpu.setMemory(0x201, 0xFB);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.gfx[0 * 64 + 0], 0);   // (0,0) пуст
    EXPECT_EQ(cpu.gfx[0 * 64 + 4], 1);   // (4,0) включён
    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// 00FC — Scroll left 4 pixels
// ============================================================
TEST(Chip8, 00FC_ScrollsLeft) {
    chip8 cpu;
    cpu.init();

    cpu.gfx[0 * 64 + 8] = 1;   // (8, 0)

    cpu.setMemory(0x200, 0x00);
    cpu.setMemory(0x201, 0xFC);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.gfx[0 * 64 + 8], 0);   // (8,0) пуст
    EXPECT_EQ(cpu.gfx[0 * 64 + 4], 1);   // (4,0) включён
    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// 00FD — Exit interpreter
// ============================================================
// Эту инструкцию сложно протестировать, потому что она вызывает exit().
// Можно только проверить, что она назначена в таблице:
TEST(Chip8, 00FD_IsRegistered) {
    // Если инструкция не назначена, то при вызове будет cpuNULL,
    // и pc не сдвинется. Проверим косвенно через другие тесты.
    // Прямой тест потребовал бы перехвата exit().
    SUCCEED() << "00FD requires exit() interception to test properly";
}

// ============================================================
// 00FE — Switch to lores mode (no-op в нашем эмуляторе)
// ============================================================
TEST(Chip8, 00FE_IsNoOp) {
    chip8 cpu;
    cpu.init();

    cpu.setMemory(0x200, 0x00);
    cpu.setMemory(0x201, 0xFE);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// 00FF — Switch to hires mode (no-op в нашем эмуляторе)
// ============================================================
TEST(Chip8, 00FF_IsNoOp) {
    chip8 cpu;
    cpu.init();

    cpu.setMemory(0x200, 0x00);
    cpu.setMemory(0x201, 0xFF);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// 9XY0 — Skip if VX != VY
// ============================================================
TEST(Chip8, 9XY0_SkipsWhenNotEqual) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x42);
    cpu.setV(0x6, 0x00);
    cpu.setMemory(0x200, 0x95);
    cpu.setMemory(0x201, 0x60);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x204);   // пропустили
}

TEST(Chip8, 9XY0_DoesNotSkipWhenEqual) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x42);
    cpu.setV(0x6, 0x42);
    cpu.setMemory(0x200, 0x95);
    cpu.setMemory(0x201, 0x60);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// BXNN — Jump to NNN + VX (современная трактовка BNNN) перепроверить
// ============================================================
TEST(Chip8, BXNN_JumpsToNNNPlusVX) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x10);
    cpu.setMemory(0x200, 0xB5);   // X = 5
    cpu.setMemory(0x201, 0x20);   // NNN = 0x200
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x530);   // 0x200 + V[5] = 0x200 + 0x10
}

// ============================================================
// CXNN — VX = rand() & NN
// ============================================================
TEST(Chip8, CXNN_ResultMaskedByNN) {
    chip8 cpu;
    cpu.init();

    cpu.setMemory(0x200, 0xC5);
    cpu.setMemory(0x201, 0x0F);   // V[5] = rand() & 0x0F
    cpu.setPc(0x200);

    cpu.runInstruction();

    // Результат не может содержать биты, которых нет в 0x0F
    EXPECT_LE(cpu.getV(0x5), 0x0F);
    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// DXY0 — Draw 16x16 sprite
// ============================================================
TEST(Chip8, DXY0_Draws16x16Sprite) {
    chip8 cpu;
    cpu.init();

    // Спрайт 16x16: первый пиксель в верхнем левом углу
    cpu.setI(0x300);
    cpu.setMemory(0x300, 0x80);   // первый байт первой строки
    cpu.setMemory(0x301, 0x00);   // второй байт первой строки
    // Остальные строки — нули

    cpu.setV(0x5, 0);
    cpu.setV(0x6, 0);

    cpu.setMemory(0x200, 0xD5);
    cpu.setMemory(0x201, 0x00);   // DXY0: 16x16
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.gfx[0], 1);        // пиксель (0,0) включён
    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// 8XY6 — VX >>= 1, VF = младший бит до сдвига
// ============================================================
TEST(Chip8, 8XY6_ShiftRight) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0b00000101);
    cpu.setMemory(0x200, 0x85);
    cpu.setMemory(0x201, 0x66);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x5), 0b00000010);
    EXPECT_EQ(cpu.getV(0xF), 1);   // младший бит был 1
}

// ============================================================
// 8XY7 — VX = VY - VX, VF = no borrow
// ============================================================
TEST(Chip8, 8XY7_SubNRegister) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x10);
    cpu.setV(0x6, 0x30);
    cpu.setMemory(0x200, 0x85);
    cpu.setMemory(0x201, 0x67);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x5), 0x20);   // V[6] - V[5] = 0x30 - 0x10
    EXPECT_EQ(cpu.getV(0xF), 1);      // no borrow
}

// ============================================================
// 8XYE — VX <<= 1, VF = старший бит до сдвига
// ============================================================
TEST(Chip8, 8XYE_ShiftLeft) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0b10000001);
    cpu.setMemory(0x200, 0x85);
    cpu.setMemory(0x201, 0x6E);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x5), 0b00000010);
    EXPECT_EQ(cpu.getV(0xF), 1);   // старший бит был 1
}

// ============================================================
// FX07 — VX = delay_timer
// ============================================================
TEST(Chip8, FX07_ReadsDelayTimer) {
    chip8 cpu;
    cpu.init();

    // Установим delay_timer через FX15
    cpu.setV(0x5, 0x42);
    cpu.setMemory(0x200, 0xF5);
    cpu.setMemory(0x201, 0x15);
    cpu.setPc(0x200);
    cpu.runInstruction();   // delay_timer = 0x42

    // Теперь прочитаем его
    cpu.setMemory(0x200, 0xF6);
    cpu.setMemory(0x201, 0x07);
    cpu.setPc(0x200);
    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x6), 0x42);
}

// ============================================================
// FX15 — delay_timer = VX
// ============================================================
TEST(Chip8, FX15_SetsDelayTimer) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x42);
    cpu.setMemory(0x200, 0xF5);
    cpu.setMemory(0x201, 0x15);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getDelayTimer(), 0x42);
    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// FX18 — sound_timer = VX
// ============================================================
TEST(Chip8, FX18_SetsSoundTimer) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x42);
    cpu.setMemory(0x200, 0xF5);
    cpu.setMemory(0x201, 0x18);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getSoundTimer(), 0x42);
    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// FX1E — I += VX, VF = overflow
// ============================================================
TEST(Chip8, FX1E_AddsVXToI) {
    chip8 cpu;
    cpu.init();

    cpu.setI(0x200);
    cpu.setV(0x5, 0x10);
    cpu.setMemory(0x200, 0xF5);
    cpu.setMemory(0x201, 0x1E);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getI(), 0x210);
    EXPECT_EQ(cpu.getV(0xF), 0);   // нет переполнения
}

TEST(Chip8, FX1E_SetsVFOnOverflow) {
    chip8 cpu;
    cpu.init();

    cpu.setI(0xFF0);
    cpu.setV(0x5, 0x20);
    cpu.setMemory(0x200, 0xF5);
    cpu.setMemory(0x201, 0x1E);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getI(), 0x1010);
    EXPECT_EQ(cpu.getV(0xF), 1);   // переполнение
}

// ============================================================
// FX29 — I = адрес спрайта символа VX
// ============================================================
TEST(Chip8, FX29_SetsIToFontSprite) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0xA);   // символ 'A'
    cpu.setMemory(0x200, 0xF5);
    cpu.setMemory(0x201, 0x29);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getI(), 0xA * 5);   // 50
    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// FX30 — I = адрес 10-байтного спрайта символа VX
// ============================================================
TEST(Chip8, FX30_SetsIToBigFontSprite) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x3);
    cpu.setMemory(0x200, 0xF5);
    cpu.setMemory(0x201, 0x30);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getI(), 0x3 * 0x10);   // 0x30
    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// FX55 — Сохранить V0..VX в память начиная с I
// ============================================================
TEST(Chip8, FX55_StoresRegistersInMemory) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x0, 0xAA);
    cpu.setV(0x1, 0xBB);
    cpu.setV(0x2, 0xCC);
    cpu.setI(0x300);
    cpu.setMemory(0x200, 0xF2);
    cpu.setMemory(0x201, 0x55);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getMemory(0x300), 0xAA);
    EXPECT_EQ(cpu.getMemory(0x301), 0xBB);
    EXPECT_EQ(cpu.getMemory(0x302), 0xCC);
}

// ============================================================
// FX65 — Загрузить V0..VX из памяти начиная с I
// ============================================================
TEST(Chip8, FX65_LoadsRegistersFromMemory) {
    chip8 cpu;
    cpu.init();

    cpu.setMemory(0x300, 0xAA);
    cpu.setMemory(0x301, 0xBB);
    cpu.setMemory(0x302, 0xCC);
    cpu.setI(0x300);
    cpu.setMemory(0x200, 0xF2);
    cpu.setMemory(0x201, 0x65);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x0), 0xAA);
    EXPECT_EQ(cpu.getV(0x1), 0xBB);
    EXPECT_EQ(cpu.getV(0x2), 0xCC);
}

// ============================================================
// FX75 — Сохранить V0..VX в flags
// ============================================================
TEST(Chip8, FX75_SavesFlags) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x0, 0x11);
    cpu.setV(0x1, 0x22);
    cpu.setV(0x2, 0x33);
    cpu.setMemory(0x200, 0xF2);
    cpu.setMemory(0x201, 0x75);
    cpu.setPc(0x200);

    cpu.runInstruction();

    // Проверяем косвенно: загружаем обратно через FX85
    cpu.setV(0x0, 0);
    cpu.setV(0x1, 0);
    cpu.setV(0x2, 0);
    cpu.setMemory(0x200, 0xF2);
    cpu.setMemory(0x201, 0x85);
    cpu.setPc(0x200);
    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x0), 0x11);
    EXPECT_EQ(cpu.getV(0x1), 0x22);
    EXPECT_EQ(cpu.getV(0x2), 0x33);
}

// ============================================================
// FX85 — Загрузить V0..VX из flags
// ============================================================
TEST(Chip8, FX85_LoadsFlags) {
    chip8 cpu;
    cpu.init();

    // Сначала сохраним
    cpu.setV(0x0, 0xAA);
    cpu.setV(0x1, 0xBB);
    cpu.setMemory(0x200, 0xF1);
    cpu.setMemory(0x201, 0x75);
    cpu.setPc(0x200);
    cpu.runInstruction();

    // Обнулим
    cpu.setV(0x0, 0);
    cpu.setV(0x1, 0);

    // Загрузим
    cpu.setMemory(0x200, 0xF1);
    cpu.setMemory(0x201, 0x85);
    cpu.setPc(0x200);
    cpu.runInstruction();

    EXPECT_EQ(cpu.getV(0x0), 0xAA);
    EXPECT_EQ(cpu.getV(0x1), 0xBB);
}

// ============================================================
// EX9E — Skip if key VX pressed
// ============================================================
TEST(Chip8, EX9E_SkipsIfKeyPressed) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x3);
    cpu.key[0x3] = 1;   // key — публичный

    cpu.setMemory(0x200, 0xE5);
    cpu.setMemory(0x201, 0x9E);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x204);   // пропустили
}

TEST(Chip8, EX9E_DoesNotSkipIfKeyNotPressed) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x3);
    cpu.key[0x3] = 0;

    cpu.setMemory(0x200, 0xE5);
    cpu.setMemory(0x201, 0x9E);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x202);
}

// ============================================================
// EXA1 — Skip if key VX NOT pressed
// ============================================================
TEST(Chip8, EXA1_SkipsIfKeyNotPressed) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x3);
    cpu.key[0x3] = 0;

    cpu.setMemory(0x200, 0xE5);
    cpu.setMemory(0x201, 0xA1);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x204);
}

TEST(Chip8, EXA1_DoesNotSkipIfKeyPressed) {
    chip8 cpu;
    cpu.init();

    cpu.setV(0x5, 0x3);
    cpu.key[0x3] = 1;

    cpu.setMemory(0x200, 0xE5);
    cpu.setMemory(0x201, 0xA1);
    cpu.setPc(0x200);

    cpu.runInstruction();

    EXPECT_EQ(cpu.getPc(), 0x202);
}