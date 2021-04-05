#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <stdint.h>

#include <gtk/gtk.h>

#define NUMBER_KEYS 16

static const uint8_t keyMappingQwerty[NUMBER_KEYS] = {
    GDK_KEY_x,
    GDK_KEY_1,
    GDK_KEY_2,
    GDK_KEY_3,
    GDK_KEY_q,
    GDK_KEY_w,
    GDK_KEY_e,
    GDK_KEY_a,
    GDK_KEY_s,
    GDK_KEY_d,
    GDK_KEY_z,
    GDK_KEY_c,
    GDK_KEY_4,
    GDK_KEY_r,
    GDK_KEY_f,
    GDK_KEY_v
};

static const int keyMappingAzertyMac[NUMBER_KEYS] = {
    GDK_KEY_x,
    GDK_KEY_ampersand,
    GDK_KEY_eacute,
    GDK_KEY_quotedbl,
    GDK_KEY_a,
    GDK_KEY_z,
    GDK_KEY_e,
    GDK_KEY_q,
    GDK_KEY_s,
    GDK_KEY_d,
    GDK_KEY_w,
    GDK_KEY_c,
    GDK_KEY_apostrophe,
    GDK_KEY_r,
    GDK_KEY_f,
    GDK_KEY_v
};

uint16_t keyPressed;
struct Chip8* displayedChip8;
const int* currentKeyMapping;

extern GtkWidget* window;

void initKeyboard(struct Chip8* chip8);

#endif // KEYBOARD_H_