#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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
uint16_t PC = START_OF_PROGRAM;
uint16_t I;
uint8_t delayTimer = 0;
uint8_t soundTimer = 0;

// can be optimized with bools or bit operations
unsigned char screen[SCREEN_HEIGHT][SCREEN_WIDTH];

uint16_t stack[32];
uint8_t SP=0;

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
    printRAM(START_OF_PROGRAM, numberBytes);

    PC+=4;

    uint16_t instruction = ((memory[PC] << 8) | memory[PC+1]);
    printf("Instruction: %.4x\n", instruction);
    uint8_t x = (instruction >> 8) & 0x000F;
    uint8_t y = (instruction >> 4) & 0x000F;

    switch(instruction & 0xF000){
        case 0x0000:
        switch(instruction){
            case 0x00E0:
            clearScreen();
            PC+=2;
            break;

            case 0x00EE:
            PC=stack[SP];
            SP--;
            break;

            default:
            printf("Instruction not handled: 0x%.4x\n", instruction);
        }
        break;

        case 0x1000:
        PC = instruction & 0x0FFF;
        break;

        case 0x2000:
        stack[++SP] = PC;
        PC = instruction & 0x0FFF;
        break;

        case 0x3000:
        if(registers[x] == instruction & 0x00FF){
            PC+=4;
        }
        else{
            PC+=2;
        }
        break;

        case 0x4000:
        if(registers[x] != instruction & 0x00FF){
            PC+=4;
        }
        else{
            PC+=2;
        }
        break;

        case 0x5000:
        if(registers[x] == registers[y]){
            PC+=4;
        }
        else{
            PC+=2;
        }
        break;

        case 0x6000:
        registers[x] = instruction & 0x00FF;
        PC+=2;
        break;

        case 0x7000:
        registers[x] += instruction & 0x00FF;
        PC+=2;
        break;

        case 0x8000:
        switch(instruction & 0x000F){
            case 0x0000:
            registers[x] = registers[y];
            PC+=2;
            break;

            case 0x0001:
            registers[x] = registers[x] | registers[y];
            PC+=2;
            break;

            case 0x0002:
            registers[x] = registers[x] & registers[y];
            PC+=2;
            break;

            case 0x0003:
            registers[x] = registers[x] ^ registers[y];
            PC+=2;
            break;

            case 0x0004:
            registers[x] = registers[x] + registers[y];
            if(registers[x] + registers[y] > 255){
                registers[VF] = 1;
            }
            else{
                registers[VF] = 0;
            }
            PC+=2;
            break;

            case 0x0005:
            registers[x] = registers[x] - registers[y];
            if(registers[x] > registers[y]){
                registers[VF] = 1;
            }
            else{
                registers[VF] = 0;
            }
            PC+=2;
            break;

            case 0x0006:
            registers[VF] = registers[x] & 0x01;
            registers[x] /= 2;
            PC+=2;
            break;

            case 0x0007:
            registers[x] = registers[y] - registers[x];
            if(registers[y] > registers[x]){
                registers[VF] = 1;
            }
            else{
                registers[VF] = 0;
            }
            PC+=2;
            break;

            case 0x000E:
            registers[VF] = registers[x] & 0x01;
            registers[x] *= 2;
            PC+=2;
            break;

            default:
            printf("Instruction not handled: 0x%.4x\n");
        }
        break;

        case 0x9000:
        switch(instruction & 0x0001){
            case 0x0000:
            if(registers[y] != registers[x]){
                PC+=4;
            }
            else{
                PC+=2;
            }
            break;

            default:
            printf("Instruction not handled: 0x%.4x\n");
        }
        break;

        case 0xA000:
        I = instruction & 0x0FFF;
        PC += 2;
        break;

        case 0xB000:
        PC = (instruction & 0x0FFF) + registers[V0];
        break;

        case 0xC000:
        srand(time(NULL));
        uint32_t randomNumber = rand() % 256;
        registers[x] = randomNumber & (instruction & 0x00FF);
        break;

        case 0xD000:
        int totalBytesToDisplay = instruction & 0x000F;
        for(int i = 0; i < totalBytesToDisplay; i++){
            
        }
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
