#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include <ncurses.h>

#define FPS 25
#define FRAME_TARGET_TIME (1000/FPS)

////////////////////////////////////////////////////////////////////
//	Global Variables
////////////////////////////////////////////////////////////////////
extern int window_width;
extern int window_height;
extern int terminal_width;
extern int terminal_height;

////////////////////////////////////////////////////////////////////
//      Functions
////////////////////////////////////////////////////////////////////
bool initialize_ncurses();
void draw_pixel(int y, int x, short color_pair_id, short fg, short bg);
int get_terminal_size_pixels(int *width, int *height); 
void draw_rect(int x, int y, int width, int height);
#endif
