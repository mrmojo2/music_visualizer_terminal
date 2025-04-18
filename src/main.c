#include <pulse/simple.h>
#include <pulse/error.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <complex.h>

#include "display.h"
#include "fft.h"

#define RATE 44100
#define N    512
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])


/////////////////////////////////////////////////////////////////////
//	Global Variables
/////////////////////////////////////////////////////////////////////
bool isRunning = false;
bool time_domain = true;
int error;
int previous_frame_time = 0;
pa_simple *s = NULL;
pa_sample_spec ss;
int16_t buffer[(RATE * 2)/86];	//stores 11ms of stereo audio
float left_buffer[N];
float right_buffer[N];
float max_amp;
float complex left_transform[N];
float complex right_transform[N];


/////////////////////////////////////////////////////////////////////
//	Function definations
/////////////////////////////////////////////////////////////////////

void setup(){
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
	
	ss.format = PA_SAMPLE_S16NE;	//16 bit sample size
	ss.rate = RATE;			//sample rate
	ss.channels = 2;		//sterio

	//connect to pulseaudio server
	s = pa_simple_new(
		NULL,
		"MUSIC_VISUALIZER",
		PA_STREAM_RECORD,
		"bluez_sink.22_22_22_64_35_77.a2dp_sink.monitor",	//readd sample from bluetooth speaker output monitor
		//"alsa_input.pci-0000_00_1f.3.analog-stereo",
		"music_vsualizer",
		&ss,
		NULL,
		NULL,
		&error
	);

	if(!s){
		fprintf(stderr,"Failed to connect: %s\n",pa_strerror(error));
		return;
	}

}

void process_input(void){
SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type){
                case SDL_QUIT:
                        isRunning = false;
                        break;
                case SDL_KEYDOWN:
                        if(event.key.keysym.sym == SDLK_ESCAPE){
                                isRunning = false;
                        }
                        if(event.key.keysym.sym == SDLK_d){
                                time_domain = !time_domain;
                        }

                        break;
        }
}

void update(void){
	//control the fps
	/*int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks()-previous_frame_time);        
	if(time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME){
		SDL_Delay(time_to_wait);
	}*/

	previous_frame_time = SDL_GetTicks();            //how may ms since SDL_INIT

	if(pa_simple_read(s,buffer,sizeof(buffer),&error) < 0){
		fprintf(stderr,"failed to read: %s,",pa_strerror(error));
		pa_simple_free(s);
	}
	
	max_amp = 0.0f;
	for(size_t i=0; i<N; ++i){
		left_buffer[i] = buffer[2*i];
		right_buffer[i] = buffer[2*i+1];
	}
if(!time_domain){
	//perform fft:
	fft(left_buffer,left_transform,N);
	fft(right_buffer,right_transform,N);
	for(size_t i=0; i<N;++i){
		float c = (cabsf(left_transform[i]));
		if( c > max_amp)
			max_amp = c;
	}
}


}

void render(void){
if(time_domain){
	for(size_t i=0; i<ARRAY_SIZE(buffer)/2;  i++){
		int16_t sample_right = buffer[2*i];
		int16_t sample_left  = buffer[2*i + 1];
		float normalized_r;
		float normalized_l;
		float end_y_r;
		float end_y_l;
		if(sample_right > 0){
			normalized_r = (float) sample_right/INT16_MAX;
			normalized_l = (float) sample_left/INT16_MAX;
			end_y_r = window_height/2 - normalized_r*500;
			end_y_l = window_height/2 - normalized_l*500;
		}else{
			normalized_r = (float)sample_right/INT16_MIN;
			normalized_l = (float)sample_left/INT16_MIN;
			end_y_r = window_height/2 + normalized_r*500;
			end_y_l = window_height/2 + normalized_l*500;
		
		}
		printf("sample_right: %d,sample_left: %d\n", sample_right,sample_left);
		draw_line(2*i, window_height/2,2*i,end_y_l, 0xffff0000);
		draw_line(2*i+1, window_height/2,2*i+1,end_y_r, 0xff0000ff);
			
	}
}else{
	float cell_width =2*(float)window_width/N;
	int h = window_height/2;
	for(size_t i = 0; i<N/2; ++i){
		float lmag= cabsf(left_transform[i])/max_amp;	
		float rmag= cabsf(right_transform[i]);

		printf("lmag: %5.5f, rmag: %5.5f\n",lmag,rmag);
		draw_line(i*cell_width,h,i*cell_width,h-h*lmag,0xffff0000);
		/*SDL_SetRenderDrawColor(renderer,255,0,0,255);
		SDL_Rect rect  = {i*cell_width,h-h*lmag,cell_width,h*lmag};
		SDL_RenderFillRect(renderer,&rect);
		SDL_RenderPresent(renderer);*/
	}

}
	render_color_buffer();
	clear_color_buffer(0xffaabbcc);
	SDL_RenderPresent(renderer);                  //This swaps the back buffer (where drawing happens) with the front buffer (displayed on the screen), making the rendered image visible to the user

}

void free_resources(void){
	destroy_window();
	free(color_buffer);
	pa_simple_free(s);
}


int main(void){

	isRunning = initialize_window();
	setup();
	while(isRunning){
		process_input();
		update();
		render();
	}
	/*while(1){
		if(pa_simple_read(s,buffer,sizeof(buffer),&error) < 0){
				fprintf(stderr,"failed to read: %s,",pa_strerror(error));
				pa_simple_free(s);
		}
		
		for(size_t i=0; i<array_size(buffer)/2; ++i){
			int16_t sample_right = buffer[i*2];//taking just one channel for now
			printf("sample: %d\n",sample_right);
		}
	
	}*/
		
	free_resources();	
	return 0;
}

