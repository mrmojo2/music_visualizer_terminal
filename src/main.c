#include <SDL2/SDL.h>
#include <stdio.h>
#include <complex.h>

#include "display.h"
#include "fft.h"
#include "audio.h"
#include "vector.h"

#define N  512
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])


/////////////////////////////////////////////////////////////////////
//	Global Variables
/////////////////////////////////////////////////////////////////////
bool isRunning = false;
int previous_frame_time = 0;
bool time_domain = true;
SDL_Event event;
audio_t audio_data;
SDL_AudioSpec wav_spec;
SDL_AudioDeviceID device_id;
float left_sample[N];
float right_sample[N];
float complex fft_out[N];

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
	audio_is_playing = true;

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
				if(event.key.keysym.sym == SDLK_SPACE){
					if(audio_is_playing){
						SDL_PauseAudioDevice(device_id,0);//start audio_ playback
						audio_is_playing = false;
					}else{
						SDL_PauseAudioDevice(device_id,1);//start audio_ playback
						audio_is_playing = true;
						
					}
				}
				if(event.key.keysym.sym == SDLK_d){
					time_domain = !time_domain;
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
	int16_t *buf =(int16_t *)(audio_data.buffer + audio_data.position);
	size_t buf_size = 2*N;

	int h_by_2 = window_height/2;
if(time_domain){
	for(size_t i = 0; i < buf_size/2; i++){
		int16_t left = buf[2*i];
		int16_t right = buf[2*i+1];
	
		float normalized_l;
		float normalized_r;
		float end_y_l;
		float end_y_r;
		if(left > 0){
			normalized_l = (float)left/INT16_MAX;
			end_y_l = h_by_2 - h_by_2*normalized_l;
		}else{
			normalized_l = (float)left/INT16_MIN;
			end_y_l = h_by_2 + h_by_2*normalized_l;
		
		}
		if(right > 0){
			normalized_r = (float)right/INT16_MAX;
			end_y_r = h_by_2 - h_by_2*normalized_r;
		}else{
			normalized_r = (float)right/INT16_MIN;
			end_y_r = h_by_2 + h_by_2*normalized_r;
		
		}
		draw_line(2*i, h_by_2, 2*i,end_y_l, 0xffff0000);
		draw_line(2*i+1, h_by_2, 2*i+1,end_y_r, 0xff0000ff);
	}
}else{
	for(size_t i=0; i < buf_size/2; i++){
		left_sample[i] = buf[2*i];
		right_sample[i] = buf[2*i+1];
	}
	
	fft(left_sample,fft_out,N);
	
	float max_amp = 0.0f;
	for(size_t i=0; i<N; i++){
		float c = cabsf(fft_out[i]); 
		if(c > max_amp)
			max_amp = c;
	}
	
	int sample_rate = wav_spec.freq;
	int max_freq = sample_rate/2;

	int num_bar = 100;
	
	float bar_mag[num_bar];
	int bar_mag_i = 0;
	memset(bar_mag,0,sizeof(bar_mag));

	float log_min = log10f(20);
	float log_max = log10f(max_freq);
	
	float prev_freq = 0;
	for(int i=0; i < num_bar;i++){
		float t = (float) i/num_bar;
		float log_freq = log_min + (log_max - log_min)*t;
		float current_freq = powf(10,log_freq);

		int current_index = (int)(current_freq/max_freq * N/2);
		int prev_index = (int)(prev_freq/max_freq * N/2);

		float sum_mag = 0.0f;
		float bins_summed = 0.0f;

		for(int j = prev_index+1 ; j <= current_index ; j++){
			if(j >=0 && j < N/2){
				float mag = cabsf(fft_out[j]);
				sum_mag += mag;
				bins_summed++;
			}
		}
		
		if(sum_mag > 1e-4){
			bar_mag[bar_mag_i++] = sum_mag;
		}
		prev_freq = current_freq;
	}
	
	int cell_width = 0;
	if(bar_mag_i > 0)
		cell_width = window_width / bar_mag_i;
	for(int i=0; i<num_bar; i++){
		float normalized = 0.0f;
		if(fabs(max_amp) > 1e-4){
			normalized = bar_mag[i]/max_amp;
		}
		float bar_height = h_by_2 - h_by_2*normalized*0.75;
		draw_line_gradient(i*cell_width,bar_height,i*cell_width,h_by_2,0xffff0000);
		draw_gradient_rect(i*cell_width,bar_height,cell_width,h_by_2 - bar_height);
	}	

	
}
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

