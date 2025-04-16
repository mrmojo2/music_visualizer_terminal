#include "display.h"

#include <stdio.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *color_buffer = NULL;
SDL_Texture *color_buffer_texture = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Eror Initializing SDL \n");
        return false;
    }

    // use sdl to query the full screen size of the display

    // Create SDL Window
    window = SDL_CreateWindow(
        NULL, // no window name
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS);

    if (!window)
    {
        fprintf(stderr, "Error Creating SDL Window\n");
        return false;
    }

    // Create SDL Renderer
    renderer = SDL_CreateRenderer(
        window,
        -1, // use default() output/display device
        0   // no special flags
    );

    if (!renderer)
    {
        fprintf(stderr, "Error creating SDL Renerer.\n");
        return false;
    }

    return true;
}

void clear_color_buffer(uint32_t color){
	for(int i=0; i<window_width * window_height; i++)
		color_buffer[i] = color;
}

void draw_pixel(int x, int y, uint32_t color){
	if(x < window_width && x>=0 && y < window_height && y >=0 ) color_buffer[window_width*y + x] = color;
}

void render_color_buffer()
{

    /*copies the pixel data from color_buffer (a raw array of pixel values) to color_buffer_texture. Essentially, it converts raw pixel data into a format that SDL can work with (i.e., a texture). */
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width * sizeof(uint32_t)));

    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL); // Copies a texture to the renderer's target (prepares the frame).
}

void destroy_window()
{
    // reverse order of allocation
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

//drawing line using Bresenham's algorithm
void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        draw_pixel(x0, y0, color); // Draw the current pixel

        if (x0 == x1 && y0 == y1) break; // End condition

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}
