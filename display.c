#include "display.h"

#include <gtk/gtk.h>
#include <cairo.h>

static int windowWidth, windowHeight;
static int squareSize = SQUARE_SIZE_INITIAL;

GtkWidget* window;
static GtkWidget* darea;

void printScreenConsole() {
    printf("\n");
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            if (displayedChip8 ->  screen[i][j] == 0) {
                printf("x");
            }
            else {
                printf(".");
            }
            printf(" ");
        }
        printf("\n");
    }
    printf("\n");
    printf("\n");
}

static void draw(cairo_t *cr) {
    cairo_set_source_rgb(cr, 0.5, 0.5, 1);

    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            if (displayedChip8 -> screen[i][j] == 1) {
                cairo_rectangle (cr, j * squareSize, i * squareSize, squareSize, squareSize);
                gtk_window_get_size(GTK_WINDOW(window), &windowWidth, &windowHeight);
            }
        }
    }
    cairo_fill(cr);
}

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    draw(cr);
    return FALSE;
}

gboolean resize_event(GtkWidget *widget, cairo_t *cr, gpointer user_data){
    gtk_window_get_size(GTK_WINDOW(window), &windowWidth, &windowHeight);
    squareSize = windowWidth / 64;
    gtk_widget_queue_draw(darea);
    return TRUE;
}


void create_file_selection_window(){
    GtkFileChooserNative *native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    
    gint res;
    
    native = gtk_file_chooser_native_new("Select CHIP-8 file", GTK_WINDOW(window), action, "_Open", "_Cancel");
    res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));

    if(res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(native);
        filename = gtk_file_chooser_get_filename(chooser);
        loadFile(filename, displayedChip8);
        g_free(filename);
    }
    
    g_object_unref(native);
}

// timer callback for refreshing the display at 60Hz
gint timeout_callback(gpointer data) {
    timeout_callback_display(displayedChip8);
    
    gtk_widget_queue_draw(darea);
    return TRUE;
}

void initDisplay(struct Chip8* chip8) {
    displayedChip8 = chip8;

    gtk_init(NULL, NULL);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(window), "Chipster2D");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), SCREEN_WIDTH * squareSize, SCREEN_HEIGHT * squareSize);
    
    GdkGeometry hints;
    
    // inc doesn't work for now
    hints.base_width = SCREEN_WIDTH * squareSize;
    hints.base_height = SCREEN_HEIGHT * squareSize;
    hints.width_inc = SCREEN_WIDTH;
    hints.height_inc = SCREEN_HEIGHT;
    hints.min_aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
    hints.max_aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
  
    darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(G_OBJECT(window), "configure-event", G_CALLBACK(resize_event), NULL);

    gtk_widget_show_all(window);

    gtk_window_set_geometry_hints (GTK_WINDOW(window), NULL, &hints, GDK_HINT_BASE_SIZE | GDK_HINT_RESIZE_INC | GDK_HINT_ASPECT);
}

void startDisplay() {
    g_timeout_add_full(G_PRIORITY_HIGH, 1000 / FPS, timeout_callback, NULL, NULL);
    gtk_main();
}