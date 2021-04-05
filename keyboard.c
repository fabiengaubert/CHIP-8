#include "keyboard.h"
#include "chip8.h"

gboolean key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    printf("pressed\n");
    while(1){}
    if (event->keyval == GDK_KEY_space) {
        printf("SPACE KEY PRESSED!\n");
        displayedChip8 -> isPaused = !displayedChip8 -> isPaused;

        return TRUE;
    }
    for(int i=0; i < NUMBER_KEYS; i++) {
        if(event->keyval == currentKeyMapping[i]) {
            keyPressed |= (1 << i);
            printf("KEY %.2x PRESSED!\n", event->keyval);
            return TRUE;
        }
    }
    return FALSE;
}

gboolean key_release_event(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    if (event->keyval == GDK_KEY_space) {
        printf("SPACE KEY RELEASED!\n");
 
        return TRUE;
    }
    for(int i=0; i < NUMBER_KEYS; i++) {
        if(event->keyval == currentKeyMapping[i]) {
            keyPressed &= ~(1 << i);
            printf("KEY %.2x RELEASED!\n", event->keyval);
            return TRUE;
        }
    }
    return FALSE;
}

void initKeyboard(struct Chip8* chip8) {
    keyPressed = 0;
    currentKeyMapping = keyMappingAzertyMac;
    displayedChip8 = chip8;
    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);
    g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK (key_press_event), NULL);
    g_signal_connect(G_OBJECT(window), "key_release_event", G_CALLBACK (key_release_event), NULL);
}