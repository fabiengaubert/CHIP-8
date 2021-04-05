#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "chip8.h"

#define FPS 60

void printScreenConsole();

void initGUI(const struct Chip8* chip8);

const struct Chip8* displayedChip8;

#endif // DISPLAY_H_