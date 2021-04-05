#include <stdlib.h>
#include <stdio.h>

#include "chip8.h"
#include "display.h"

// $  gcc `pkg-config --cflags gtk+-3.0` -o main main.c `pkg-config --libs gtk+-3.0`
int main(int argc, char** argv) {
    if(argc == 1) {
        printf("Specify a path for the ROM!\n");
        return 1;
    }
    
    struct Chip8 chip8;

    initChip8(&chip8);
    int numberBytes = loadFile(argv[1], &chip8);

    if(numberBytes == 0){
        printf("Can't load %s!\n", argv[1]);
        return 1;
    }

    initGUI(&chip8);

    return 0;
}