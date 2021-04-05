#include "chip8.h"
#include "keyboard.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const uint8_t fonts[FONTS_SIZE] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void loadFonts(struct Chip8* chip8) {
    for (int i = 0; i < FONTS_SIZE; i++) {
        chip8 -> memory[START_OF_FONT + i] = fonts[i];
    }
}

int loadFile(const char *path, struct Chip8* chip8) {
    FILE *fp = fopen(path, "rb");
    if (fp == NULL) {
        printf("erreur\n");
        return 0;
    }
    size_t numberBytesRead = fread((uint8_t *)chip8 -> memory + START_OF_PROGRAM, sizeof(uint8_t), MEMORY_SIZE - START_OF_PROGRAM, fp);
    printf("%zu bytes are loaded in memory\n", numberBytesRead);
    fclose(fp);
    return numberBytesRead;
}

void initChip8(struct Chip8* chip8) {
    chip8 -> PC = START_OF_PROGRAM;

    chip8 -> delayTimer = 0;
    chip8 -> soundTimer = 0;

    chip8 -> SP = 0;
    chip8 -> isPaused = 0;

    for (int i = 0; i < REGISTER_COUNT; i++) {
        chip8 -> registers[i] = 0;
    }

    for (int i = 0; i < MEMORY_SIZE; i++) {
        chip8 -> memory[i] = 0x00;
    }

    loadFonts(chip8);

    initKeyboard(chip8);
}

void printRAM(struct Chip8* chip8, uint16_t adressStart, int number) {
    for (int i = 0; i < number && (adressStart + i) < MEMORY_SIZE; i++) {
        printf("%.2x\n", chip8 -> memory[adressStart + i]);
    }
}

static void clearScreen(struct Chip8* chip8) {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            chip8 ->  screen[i][j] = 0;
        }
    }
}

static void decodeNextInstruction(struct Chip8* chip8) {
    uint16_t instruction = ((chip8 -> memory[chip8 -> PC] << 8) | chip8 -> memory[chip8 -> PC + 1]);
    printf("Instruction: %.4x\n", instruction);
    uint8_t x = (instruction >> 8) & 0x000F;
    uint8_t y = (instruction >> 4) & 0x000F;

    chip8 -> PC += 2;

    switch (instruction & 0xF000) {
    case 0x0000:
        switch (instruction) {
        case 0x00E0:
            clearScreen(chip8);
            break;

        case 0x00EE:
            chip8 -> PC = chip8 -> stack[--(chip8 -> SP)];
            if(chip8 -> SP<0){
                printf("Erreur, SP is negatif\n");
            }
            break;

        default:
            printf("Instruction not handled: 0x%.4x\n", instruction);
        }
        break;

    case 0x1000:
        chip8 -> PC = instruction & 0x0FFF;
        printf("Jump to %x\n", instruction & 0x0FFF);
        break;

    case 0x2000:
        chip8 -> stack[(chip8 -> SP)++] = chip8 -> PC;
        chip8 -> PC = instruction & 0x0FFF;
        break;

    case 0x3000:
        if (chip8 -> registers[x] == (instruction & 0x00FF)) {
            chip8 -> PC += 2;
        }
        break;

    case 0x4000:
        if (chip8 -> registers[x] != (instruction & 0x00FF)) {
            chip8 -> PC += 2;
        }
        break;

    case 0x5000:
        if (chip8 -> registers[x] == chip8 -> registers[y]) {
            chip8 -> PC += 2;
        }
        break;

    case 0x6000:
        chip8 -> registers[x] = instruction & 0x00FF;
        printf("Number: %.2x into register: %d\n", instruction & 0x00FF, x);
        break;

    case 0x7000:
        chip8 -> registers[x] += instruction & 0x00FF;
        break;

    case 0x8000:
        switch (instruction & 0x000F) {
        case 0x0000:
            chip8 -> registers[x] = chip8 -> registers[y];
            break;

        case 0x0001:
            chip8 -> registers[x] = chip8 -> registers[x] | chip8 -> registers[y];
            break;

        case 0x0002:
            chip8 -> registers[x] = chip8 -> registers[x] & chip8 -> registers[y];
            break;

        case 0x0003:
            chip8 -> registers[x] = chip8 -> registers[x] ^ chip8 -> registers[y];
            break;

        case 0x0004:
            chip8 -> registers[x] = chip8 -> registers[x] + chip8 -> registers[y];
            if (chip8 -> registers[x] + chip8 -> registers[y] > 255) {
                chip8 -> registers[VF] = 1;
            }
            else {
                chip8 -> registers[VF] = 0;
            }
            break;

        case 0x0005:
            chip8 -> registers[x] = chip8 -> registers[x] - chip8 -> registers[y];
            if (chip8 -> registers[x] > chip8 -> registers[y]) {
                chip8 -> registers[VF] = 1;
            }
            else {
                chip8 -> registers[VF] = 0;
            }
            break;

        case 0x0006:
            chip8 -> registers[VF] = chip8 -> registers[x] & 0x01;
            chip8 -> registers[x] /= 2;
            break;

        case 0x0007:
            chip8 -> registers[x] = chip8 -> registers[y] - chip8 -> registers[x];
            if (chip8 -> registers[y] > chip8 -> registers[x]) {
                chip8 -> registers[VF] = 1;
            }
            else {
                chip8 -> registers[VF] = 0;
            }
            break;

        case 0x000E:
            chip8 -> registers[VF] = chip8 -> registers[x] & 0x01;
            chip8 -> registers[x] *= 2;
            break;

        default:
            printf("0x8000 Instruction not handled: 0x%.4x\n", instruction);
        }
        break;

    case 0x9000:
        switch (instruction & 0x0001) {
        case 0x0000:
            if (chip8 -> registers[y] != chip8 -> registers[x]) {
                chip8 -> PC += 2;
            }
            break;

        default:
            printf("0x9000 Instruction not handled: 0x%.4x\n", instruction);
        }
        break;

    case 0xA000:
        chip8 -> I = instruction & 0x0FFF;
        break;

    case 0xB000:
        chip8 -> PC = (instruction & 0x0FFF) + chip8 -> registers[V0];
        break;

    case 0xC000:
        srand(time(NULL));
        uint32_t randomNumber = rand() % 256;
        chip8 -> registers[x] = randomNumber & (instruction & 0x00FF);
        printf("Random number generated: %.1x\n", randomNumber & (instruction & 0x00FF));
        break;

    case 0xD000:
        chip8 -> registers[VF] = 0;
        int numberBytes = instruction & 0x000F;
        printf("Coordinates for printing, x = %d, y = %d \n", chip8 -> registers[x], chip8 -> registers[y]);
        for (int i = 0; i < numberBytes; i++) {
            for (int j = 0; j < 8; j++) {
                if (chip8 -> memory[chip8 -> I + i] & (0x80 >> j)) {
                    if (chip8 -> screen[(chip8 -> registers[y] + i) % SCREEN_HEIGHT][(chip8 -> registers[x] + j) % SCREEN_WIDTH] == 1) {
                        chip8 -> registers[VF] = 1;
                        printf("Collision\n");
                    }
                    chip8 -> screen[chip8 -> registers[y] + i][chip8 -> registers[x] + j] ^= 1;
                }
            }
        }
        break;

    case 0xE000:
        switch(instruction & 0x00FF){
            case 0x009E:
                if(((keyPressed >> chip8 -> registers[x]) & 0x01) == 1){
                    chip8 -> PC += 2;
                }
            break;

            case 0x00A1:
                if(((keyPressed >> chip8 -> registers[x]) & 0x01) == 0){
                    chip8 -> PC += 2;
                }
            break;

            default:
            printf("0xE000 Instruction not handled: 0x%.4x\n", instruction);
        }
        break;

    case 0xF000:
        switch(instruction & 0x00FF){
            case 0x0000:
                // game over
                break;

            case 0x0007:
                chip8 -> registers[x] = chip8 -> delayTimer;
                break;

            case 0x000A:
                if(keyPressed != 0){
                    int i = 0;
                    while(((keyPressed >> i) & 0x01) == 0){
                        i++;
                    }
                    chip8 -> registers[x] = currentKeyMapping[i];
                }
                else{
                    chip8 -> PC -= 2;
                }
                break;

            case 0x0015:
                chip8 -> delayTimer = chip8 -> registers[x];
                break;
            
            case 0x001E:
                chip8 -> I = chip8 -> I + chip8 -> registers[x];
                break;

            case 0x0029:
                chip8 -> I = START_OF_FONT + chip8 -> registers[x] * 5;
                break;

            case 0x0033:
                chip8 -> memory[chip8 -> I] = chip8 -> registers[x] / 100;
                chip8 -> memory[chip8 -> I+1] = (chip8 -> registers[x] % 100) / 10;
                chip8 -> memory[chip8 -> I+2] = chip8 -> registers[x] % 10;
                break;

            case 0x0055:
                for(int i=0; i<=x; i++){
                    chip8 -> memory[chip8 -> I+i] = chip8 -> registers[i];
                }
                break;
            
            case 0x0065:
                for(int i=0; i<=x; i++){
                    chip8 -> registers[i] = chip8 -> memory[chip8 -> I+i];
                }
                break;

            default:
            printf("0xF000 Instruction not handled: 0x%.4x\n", instruction);
        }
        break;

    default:
        printf("Instruction doesn't exist\n");
    }
}

void timeout_callback_display(struct Chip8* chip8) {
    if(chip8 -> delayTimer != 0) {
        chip8 -> delayTimer--;
    }
    
    // CHIP-8 CPU should run at about 500Hz, hence 9 instructions are executed for every display refresh (9 * 60 = 540)
    for(int i = 0; i < 9; i++) {
        if(!chip8 -> isPaused) {
            decodeNextInstruction(chip8);
        }
    }
}