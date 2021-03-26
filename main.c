#include <stdlib.h>
#include <stdio.h>

#define MEMORY_SIZE 4096
#define START_OF_PROGRAM 0x200
#define START_OF_FONT 0x050

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

const char* path="seven.ch8";

uint8_t memory[MEMORY_SIZE];

enum RegistersNames{
    V0=0,
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

uint8_t registers[REGISTER_COUNT];
uint16_t PC=0x200;
uint16_t I;
uint8_t delayTimer=0;
uint8_t soundTimer=0;

// can be optimized with bools or bit operations
unsigned char screen[SCREEN_HEIGHT][SCREEN_WIDTH];

uint8_t stack[32];
uint8_t SP=0x00;

void init(){
    //init registers to 0
    for(int i=0; i<REGISTER_COUNT; i++){
        registers[i]=0;
    }
    //init screen for test
    for(int i=0; i<SCREEN_HEIGHT; i++){
        for(int j=0; j<SCREEN_WIDTH; j++){
            screen[i][j]='.';
        }
    }
}

void printScreen(){
    for(int i=0; i<SCREEN_HEIGHT; i++){
        for(int j=0; j<SCREEN_WIDTH; j++){
            printf("%c", screen[i][j]);
        }
        printf("\n");
    }
}

void printRAM(uint16_t adressStart, int number){
    for(int i=0; i<number && (adressStart+i)<MEMORY_SIZE; i++){
        printf("%.2x\n", memory[adressStart+i]);
    }
}

int loadFile(const char* path){
    FILE* fp=fopen(path, "rb");
    if(!fp){
        printf("erreur\n");
        return 1;
    }
    size_t numberBytesRead = fread((uint8_t*) memory+START_OF_PROGRAM, sizeof(uint8_t), MEMORY_SIZE-START_OF_PROGRAM, fp);
    fclose(fp);
    return 0;
}

void test(){
    //init RAM to 0 for test
    for(int i=0; i<MEMORY_SIZE; i++){
        memory[i]=0x00;
    }
}

int main(){
    init();
    screen[16][32]='X';
    screen[15][32]='X';
    test();
    loadFile(path);
    //printScreen();
    printRAM(0x200, 14);
    return 0;
}
