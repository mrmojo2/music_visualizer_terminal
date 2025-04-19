#include <SDL2/SDL.h>
#include <stdio.h>
#include <complex.h>

#include "display.h"
#include "fft.h"
#include "audio.h"

#define N    512
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])


/////////////////////////////////////////////////////////////////////
//	Global Variables
/////////////////////////////////////////////////////////////////////
bool isRunning = false;
int previous_frame_time = 0;
SDL_Event event;
audio_t audio_data;
SDL_AudioSpec wav_spec;
SDL_AudioDeviceID device_id;


/////////////////////////////////////////////////////////////////////
//	Function definations
/////////////////////////////////////////////////////////////////////

void setup(const char *file_path){
	//allocate memory for color buffer
	color_buffer =(uint32_t *)malloc(sizeof(uint32_t)*window_height*window_width);

	//Creating a SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);
	

	if(SDL_LoadWAV(file_path,&wav_spec,&audio_data.buffer,&audio_data.length) == NULL ){
		fprintf(stderr,"Error: %s\n",SDL_GetError());
		exit(0);
	}
	printf("frequencies: %d\n",wav_spec.freq);
	printf("format:      %s\n",get_audio_format_name(wav_spec.format));
	printf("samples:     %u\n",wav_spec.samples);
	printf("channels:    %u\n",wav_spec.channels);
	printf("length:      %u\n",audio_data.length);
	
	audio_data.position = 0;
	audio_data.format = wav_spec.format;
	wav_spec.callback = audio_callback;
	wav_spec.userdata = &audio_data;
	wav_spec.samples = N;

	device_id = SDL_OpenAudioDevice(
		NULL,//default output device
		0,   //0 = playback, 1 = recording
		&wav_spec,
		NULL,//Audio Format
		0    //Allow Changes		
	);
	if(device_id == 0){
		printf("Error: %s\n",SDL_GetError());
		SDL_FreeWAV(audio_data.buffer);
		isRunning = false;
		return;
	}
	SDL_PauseAudioDevice(device_id,0);//start audio_ playback

}

void process_input(void){
        while(SDL_PollEvent(&event) !=0){
		switch (event.type){
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE){
					isRunning = false;
				}
				if(event.key.keysym.sym == SDLK_d){
				}

				break;
			case SDL_MOUSEBUTTONDOWN:
				if(event.button.button == SDL_BUTTON_LEFT){
					int mouse_x, mouse_y;
					SDL_GetMouseState(&mouse_x, &mouse_y);
					if(mouse_y >=0 && mouse_y <=50){
						is_dragging = true;
						SDL_GetGlobalMouseState(&drag_x, &drag_y);
						int window_x,window_y;
						SDL_GetWindowPosition(window,&window_x, &window_y);
						drag_x -= window_x;
						drag_y -= window_y;
					}
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if(event.button.button == SDL_BUTTON_LEFT){
					is_dragging= false;
				}
				break;
			case SDL_MOUSEMOTION:
				if(is_dragging){
					int mouse_x, mouse_y;
					SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
					SDL_SetWindowPosition(window,mouse_x-drag_x, mouse_y-drag_y);
				}
				break;
		}
	}
}

void update(void){
	//control the fps
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks()-previous_frame_time);        
	if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
		SDL_Delay(time_to_wait);
	}

	previous_frame_time = SDL_GetTicks();            //how may ms since SDL_INIT

}

void render(void){
	//printf("%d\n",count);	
	uint16_t *s =(uint16_t *)audio_data.sample_buffer;
	size_t ss = audio_data.sample_buffer_size;
	render_color_buffer();
	clear_color_buffer(0xffaabbcc);
	SDL_RenderPresent(renderer);                  //This swaps the back buffer (where drawing happens) with the front buffer (displayed on the screen), making the rendered image visible to the user

}

void free_resources(void){
	SDL_CloseAudioDevice(device_id);
	SDL_FreeWAV(audio_data.buffer);
	destroy_window();
	free(color_buffer);
}


int main(int argc, char **argv){

	if(argc < 2 ){
		fprintf(stderr,"Usage: visualizer path_to_audio_file\n");
		exit(0);
	}
	
	isRunning = initialize_window();
	setup(argv[1]);
	while(isRunning){
		process_input();
		update();
		render();
	}
		
	free_resources();	
	return 0;
}

