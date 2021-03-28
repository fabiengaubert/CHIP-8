#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <gtk/gtk.h>
#include <cairo.h>

#define MEMORY_SIZE 4096
#define START_OF_PROGRAM 0x200
#define START_OF_FONT 0x050

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

#define FONTS_SIZE 80

#define SQUARE_SIZE 18


//const char *path = "seven.ch8";
//const char *path = "test_opcode.ch8";
//const char *path = "BCD_display.txt";
const char *path = "spaceInvaders.ch8";

uint8_t memory[MEMORY_SIZE];

enum RegistersNames
{
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

uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH];

uint16_t stack[32];
uint8_t SP = 0;

uint8_t stop = 0;

GtkWidget* window;
GtkWidget *darea;

void printScreen()
{
    printf("\n");
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            if (screen[i][j] == 0)
            {
                printf("x");
            }
            else
            {
                printf(".");
            }
            printf(" ");
        }
        printf("\n");
    }
    printf("\n");
    printf("\n");
}

void clearScreen()
{
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (int j = 0; j < SCREEN_WIDTH; j++)
        {
            screen[i][j] = 0;
        }
    }
}

void printRAM(uint16_t adressStart, int number)
{
    for (int i = 0; i < number && (adressStart + i) < MEMORY_SIZE; i++)
    {
        printf("%.2x\n", memory[adressStart + i]);
    }
}

int loadFile(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (fp == NULL)
    {
        printf("erreur\n");
        return 0;
    }
    size_t numberBytesRead = fread((uint8_t *)memory + START_OF_PROGRAM, sizeof(uint8_t), MEMORY_SIZE - START_OF_PROGRAM, fp);
    printf("%zu bytes are loaded in memory\n", numberBytesRead);
    fclose(fp);
    return numberBytesRead;
}

void init()
{
    //init registers to 0
    for (int i = 0; i < REGISTER_COUNT; i++)
    {
        registers[i] = 0;
    }
    //init RAM to 0 for test
    for (int i = 0; i < MEMORY_SIZE; i++)
    {
        memory[i] = 0x00;
    }
}

void loadFonts()
{
    for (int i = 0; i < FONTS_SIZE; i++)
    {
        memory[START_OF_FONT + i] = fonts[i];
    }
}

void decodeNextInstruction()
{
    uint16_t instruction = ((memory[PC] << 8) | memory[PC + 1]);
    printf("Instruction: %.4x\n", instruction);
    uint8_t x = (instruction >> 8) & 0x000F;
    uint8_t y = (instruction >> 4) & 0x000F;

    PC += 2;

    switch (instruction & 0xF000)
    {
    case 0x0000:
        switch (instruction)
        {
        case 0x00E0:
            clearScreen();
            break;

        case 0x00EE:
            PC = stack[--SP];
            if(SP<0){
                printf("Erreur, SP is negatif\n");
            }
            break;

        default:
            printf("Instruction not handled: 0x%.4x\n", instruction);
        }
        break;

    case 0x1000:
        PC = instruction & 0x0FFF;
        printf("Jump to %x\n", instruction & 0x0FFF);
        break;

    case 0x2000:
        stack[SP++] = PC;
        PC = instruction & 0x0FFF;
        break;

    case 0x3000:
        if (registers[x] == (instruction & 0x00FF))
        {
            PC += 2;
        }
        break;

    case 0x4000:
        if (registers[x] != (instruction & 0x00FF))
        {
            PC += 2;
        }
        break;

    case 0x5000:
        if (registers[x] == registers[y])
        {
            PC += 2;
        }
        break;

    case 0x6000:
        registers[x] = instruction & 0x00FF;
        printf("Number: %.2x into register: %d\n", instruction & 0x00FF, x);
        break;

    case 0x7000:
        registers[x] += instruction & 0x00FF;
        break;

    case 0x8000:
        switch (instruction & 0x000F)
        {
        case 0x0000:
            registers[x] = registers[y];
            break;

        case 0x0001:
            registers[x] = registers[x] | registers[y];
            break;

        case 0x0002:
            registers[x] = registers[x] & registers[y];
            break;

        case 0x0003:
            registers[x] = registers[x] ^ registers[y];
            break;

        case 0x0004:
            registers[x] = registers[x] + registers[y];
            if (registers[x] + registers[y] > 255)
            {
                registers[VF] = 1;
            }
            else
            {
                registers[VF] = 0;
            }
            break;

        case 0x0005:
            registers[x] = registers[x] - registers[y];
            if (registers[x] > registers[y])
            {
                registers[VF] = 1;
            }
            else
            {
                registers[VF] = 0;
            }
            break;

        case 0x0006:
            registers[VF] = registers[x] & 0x01;
            registers[x] /= 2;
            break;

        case 0x0007:
            registers[x] = registers[y] - registers[x];
            if (registers[y] > registers[x])
            {
                registers[VF] = 1;
            }
            else
            {
                registers[VF] = 0;
            }
            break;

        case 0x000E:
            registers[VF] = registers[x] & 0x01;
            registers[x] *= 2;
            break;

        default:
            printf("0x8000 Instruction not handled: 0x%.4x\n", instruction);
        }
        break;

    case 0x9000:
        switch (instruction & 0x0001)
        {
        case 0x0000:
            if (registers[y] != registers[x])
            {
                PC += 2;
            }
            break;

        default:
            printf("0x9000 Instruction not handled: 0x%.4x\n", instruction);
        }
        break;

    case 0xA000:
        I = instruction & 0x0FFF;
        break;

    case 0xB000:
        PC = (instruction & 0x0FFF) + registers[V0];
        break;

    case 0xC000:
        srand(time(NULL));
        uint32_t randomNumber = rand() % 256;
        registers[x] = randomNumber & (instruction & 0x00FF);
        printf("Random number generated: %.1x\n", randomNumber & (instruction & 0x00FF));
        break;

    case 0xD000:
        registers[VF] = 0;
        int numberBytes = instruction & 0x000F;
        printf("Coordinates for printing, x = %d, y = %d \n", registers[x], registers[y]);
        for (int i = 0; i < numberBytes; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (memory[I + i] & (0x80 >> j))
                {
                    if (screen[(registers[y] + i) % SCREEN_HEIGHT][(registers[x] + j) % SCREEN_WIDTH] == 1)
                    {
                        registers[VF] = 1;
                        printf("Collision\n");
                    }
                    screen[registers[y] + i][registers[x] + j] ^= 1;
                }
            }
        }
        break;

    case 0xE000:
        switch(instruction & 0x00FF){
            case 0x00A1:
                PC+=2;
                printf("Instruction to implement: 0x%.4x\n", instruction);
            break;

            default:
            printf("0xE000 Instruction not handled: 0x%.4x\n", instruction);
        }
        break;

    case 0xF000:
        switch(instruction & 0x00FF){
            case 0x0000:
                stop = 1;
                break;

            case 0x001E:
                I = I + registers[x];
                break;

            case 0x0029:
                I = START_OF_FONT + registers[x] * 5;
                break;

            case 0x0033:
                memory[I] = registers[x] / 100;
                memory[I+1] = (registers[x] % 100) / 10;
                memory[I+2] = registers[x] % 10;
                break;

            case 0x0055:
                for(int i=0; i<=x; i++){
                    memory[I+i] = registers[i];
                }
                break;
            
            case 0x0065:
                for(int i=0; i<=x; i++){
                    registers[i] = memory[I+i];
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

static void draw(cairo_t *cr){
    int width, height;
    //static size for now
    //gtk_window_get_size(GTK_WINDOW(window), &width, &height);
    cairo_set_source_rgb(cr, 0.5, 0.5, 1);
    cairo_rectangle (cr, 0, 0,SQUARE_SIZE,SQUARE_SIZE);
    cairo_fill(cr);
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
  draw(cr);
  return FALSE;
}

// $  gcc `pkg-config --cflags gtk+-3.0` -o main main.c `pkg-config --libs gtk+-3.0`
int main(int argc, char** argv)
{
    init();
    int numberBytes = loadFile(path);
    loadFonts();

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(window), "Chipster2D");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), SCREEN_WIDTH * SQUARE_SIZE, SCREEN_HEIGHT * SQUARE_SIZE); 

    darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    /*while(!stop){
        decodeNextInstruction();
    }*/

    for(int i = 0; i < 205; i++){
        decodeNextInstruction();
    }
    
  

    printScreen();

    //printRAM(START_OF_PROGRAM, numberBytes);

    return 0;
}

