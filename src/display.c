#define _XOPEN_SOURCE_EXTENDED 1
#include "display.h"
#include <stdio.h>
#include <ncurses.h>
#include <locale.h>
#include <wchar.h>
#include <unistd.h>
#include <string.h>

int window_height,window_width;               //in pixels
int terminal_height,terminal_width;	      //in charecters

bool initialize_ncurses(){
	setlocale(LC_ALL, "");
	initscr();
	start_color();
	use_default_colors(); // <<== enables transparent background
	noecho();
	curs_set(FALSE);
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	timeout(0);

	getmaxyx(stdscr,terminal_height,terminal_width);
	return true;
}



int get_terminal_size_pixels(int *width, int *height) {
	FILE *fp = popen("xwininfo -id $(xdotool getactivewindow)", "r");
	if (!fp) {
		perror("popen");
		return -1;
	}

	char line[256];
	while (fgets(line, sizeof(line), fp)) {
		if (sscanf(line, "  Width: %d", width) == 1) continue;
		if (sscanf(line, "  Height: %d", height) == 1) continue;
	}

	pclose(fp);
	return (*width > 0 && *height > 0) ? 0 : -1;
}


void draw_pixel(int y, int x, short color_pair_id, short fg, short bg) {
	init_pair(color_pair_id, fg, bg);

	wchar_t wch[] = L"█";
	cchar_t block;
	setcchar(&block, wch, A_NORMAL, color_pair_id, NULL);

	if (x >= 0 && x < window_width && y >= 0 && y < window_height) {
		mvadd_wch(y, x, &block);
	}
}

void draw_rect(int x, int y, int width, int height){
	for(int i = x; i < x+width; i++){
		for(int j = y; j < y+height; j++){
			float t = (float) j / (float)terminal_height;
			short color;
			short pair_id;
			if(t < 0.2f){
				color = COLOR_RED;
				pair_id = 1;
			}else if (t > 0.2f && t < 0.4f){
				color = COLOR_YELLOW;
				pair_id = 2;
			}else if(t > 0.4f){
				color = COLOR_GREEN;
				pair_id = 3;
			}
			draw_pixel(j,i,pair_id,color,-1);
		}
	}
}


#if 0

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

void draw_line_gradient(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (1) {
	float t = 2 * (float)y0/window_height;
	uint32_t c = 0xffff0000 + (0xffffaaaa - 0xffff0000)*t;
        draw_pixel(x0, y0,c); // Draw the current pixel

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

void draw_gradient_rect(int x, int y, int w, int h){
	for(int i = x; i < x+w ; i++){
		draw_line_gradient(i,y,i,y+h,0xff000000);
	}
}
void draw_outline_rect(int x, int y, int w, int h,uint32_t color){
	draw_line(x,y,x+w,y,color);
	draw_line(x,y,x,y+h,color);
	draw_line(x+w,y,x+w,y+h,color);
}

void draw_filled_rectangle(int x, int y, int w, int h, uint32_t color) {
    for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; ++i) {
            draw_pixel(x + i, y + j, color);
        }
    }
}

#endif
