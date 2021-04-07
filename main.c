#include <stdlib.h>
#include <stdio.h>

#include "chip8.h"
#include "display.h"
#include "keyboard.h"

// $  gcc `pkg-config --cflags gtk+-3.0` -o main main.c `pkg-config --libs gtk+-3.0`
int main(int argc, char** argv) {

    struct Chip8 chip8;

    initChip8(&chip8);
    initDisplay(&chip8);
    initKeyboard(&chip8);

    if(argc == 1) {
        create_file_selection_window();
    }
    else {
        int numberBytes = loadFile(argv[1], &chip8);
        if(numberBytes == 0){
            create_file_selection_window();
        }
    }
    
    startDisplay();

    return 0;
}