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


