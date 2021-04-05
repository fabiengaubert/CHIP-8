#ifndef CHIP8_H_
#define CHIP8_H_

#include <stdint.h>

#define MEMORY_SIZE 4096
#define START_OF_PROGRAM 0x200
#define START_OF_FONT 0x050

#define FONTS_SIZE 80

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

#define SQUARE_SIZE_INITIAL 20

enum RegistersNames {
    V0 = 0,
    V1,
    V2,
    V3,
    V4,
    V5,
    V6,
    V7,
    V8,
    V9,
    VA,
    VB,
    VC,
    VD,
    VE,
    VF,
    REGISTER_COUNT
};

struct Chip8{
    uint8_t memory[MEMORY_SIZE];

    uint8_t registers[REGISTER_COUNT];
    uint16_t PC;
    uint16_t I;
    uint8_t delayTimer;
    uint8_t soundTimer;

    uint16_t stack[32];
    uint8_t SP;

    uint8_t isPaused;

    uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];
};

extern uint16_t keyPressed;
extern const int* currentKeyMapping;

int loadFile(const char *path, struct Chip8* chip8);
void printRAM(struct Chip8* chip8, uint16_t adressStart, int number);
void timeout_callback_display(struct Chip8* chip8);
void initChip8(struct Chip8* chip8);

static void decodeNextInstruction(struct Chip8* chip8);

#endif // CHIP8_H_