#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#define FPS 25
#define FRAME_TARGET_TIME (1000/FPS)

////////////////////////////////////////////////////////////////////
//	Global Variables
////////////////////////////////////////////////////////////////////
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern uint32_t *color_buffer;
extern SDL_Texture *color_buffer_texture;
extern int window_width;
extern int window_height;


////////////////////////////////////////////////////////////////////
//      Functions
////////////////////////////////////////////////////////////////////
bool initialize_window(void);

void draw_pixel(int x,int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_filled_rectangle(int x, int y, int w, int h, uint32_t color); 

void clear_color_buffer(uint32_t color);
void render_color_buffer(void);
void destroy_window(void);

#endif
