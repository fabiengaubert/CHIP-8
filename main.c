#include <stdlib.h>
#include <stdio.h>

#define MEMORY_SIZE 4096
#define START_OF_PROGRAM 0x200
#define START_OF_FONT 0x050

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

#define FONTS_SIZE 80

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

uint8_t fonts[FONTS_SIZE] =
{
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

uint8_t registers[REGISTER_COUNT];
uint16_t PC=0x200;
uint16_t I;
uint8_t delayTimer=0;
uint8_t soundTimer=0;

// can be optimized with bools or bit operations
unsigned char screen[SCREEN_HEIGHT][SCREEN_WIDTH];

uint16_t stack[32];
uint8_t SP=0x00;

void printScreen(){
    for(int i=0; i<SCREEN_HEIGHT; i++){
        for(int j=0; j<SCREEN_WIDTH; j++){
            printf("%c", screen[i][j]);
        }
        printf("\n");
    }
}

void clearScreen(){
    for(int i=0; i<SCREEN_HEIGHT; i++){
        for(int j=0; j<SCREEN_WIDTH; j++){
            screen[i][j]='.';
        }
    }
}

void printRAM(uint16_t adressStart, int number){
    for(int i=0; i<number && (adressStart+i)<MEMORY_SIZE; i++){
        printf("%.2x\n", memory[adressStart+i]);
    }
}

int loadFile(const char* path){
    FILE* fp=fopen(path, "rb");
    if(fp==NULL){
        printf("erreur\n");
        return 0;
    }
    size_t numberBytesRead = fread((uint8_t*) memory+START_OF_PROGRAM, sizeof(uint8_t), MEMORY_SIZE-START_OF_PROGRAM, fp);
    printf("%d bytes are loaded in memory\n", numberBytesRead);
    fclose(fp);
    return numberBytesRead;
}

void init(){
    //init registers to 0
    for(int i=0; i<REGISTER_COUNT; i++){
        registers[i]=0;
    }
    clearScreen();
    //init RAM to 0 for test
    for(int i=0; i<MEMORY_SIZE; i++){
        memory[i]=0x00;
    }
}

void loadFonts(){
    for(int i=0; i<FONTS_SIZE; i++){
        memory[START_OF_FONT+i]=fonts[i];
    }
}





int main(){
    init();
    int numberBytes=loadFile(path);
    loadFonts();

    //printScreen();
    printRAM(0x200, numberBytes);


    uint16_t instruction=memory[PC];
    switch(instruction&0xF000){
        case 0x0000:
        switch(instruction){
            case 0x00E0:
            clearScreen();
            break;
        }
        
        break;

        case 0x1000:
        
        break;

        case 0x2000:
        
        break;

        case 0x3000:
        
        break;

        case 0x4000:
        
        break;

        case 0x5000:
        
        break;

        case 0x6000:
        
        break;

        case 0x7000:
        
        break;

        case 0x8000:
        
        break;

        case 0x9000:
        
        break;

        case 0xA000:
        
        break;

        case 0xB000:
        
        break;

        case 0xC000:
        
        break;

        case 0xD000:
        
        break;

        case 0xE000:
        
        break;

        case 0xF000:
        
        break;

        default:
        printf("Instruction doesn't exist\n");

    }
    return 0;
}


/*
00e0 	clear the screen
661e 	put the hex number 1e into register 6 ... the X screen coordinate is now 30 decimal
670a 	put the hex number 0a into register 7 ... the Y screen coordinate is now 10 decimal
6807 	put the hex number 7 into register 8 ... this is the number we want to display
f829 	point the index register at the graphic representing the number in register 8
d675 	display 5 graphic lines (i.e. the 7 sprite) at the location stored in registers 6 and 7
f000*/
