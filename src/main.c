#include <SDL2/SDL.h>
#include <stdio.h>
#include <unistd.h>
#include <complex.h>

#include "display.h"
#include "fft.h"
#include "audio.h"
#include "vector.h"

#define N  1024
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])


/////////////////////////////////////////////////////////////////////
//	Global Variables
/////////////////////////////////////////////////////////////////////
bool isRunning = false;
SDL_Event event;
audio_t audio_data;
SDL_AudioSpec wav_spec;
SDL_AudioDeviceID device_id;
float left_sample[N];
float right_sample[N];
float complex fft_out_left[N];
float complex fft_out_right[N];

/////////////////////////////////////////////////////////////////////
//	Function definations
/////////////////////////////////////////////////////////////////////

void free_resources(void){
	SDL_CloseAudioDevice(device_id);
	SDL_FreeWAV(audio_data.buffer);
	SDL_Quit();
	endwin();
}

void setup(const char *file_path){
	if(SDL_LoadWAV(file_path,&wav_spec,&audio_data.buffer,&audio_data.length) == NULL ){
		fprintf(stderr,"Error: %s\n",SDL_GetError());
		free_resources();	
		exit(0);
	}
	
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
	int ch = getch();
	switch(ch){
		case ' ':	
			if(audio_is_playing){
				SDL_PauseAudioDevice(device_id,0);//start audio_ playback
				audio_is_playing = false;
			}else{
				SDL_PauseAudioDevice(device_id,1);//start audio_ playback
				audio_is_playing = true;
				
			}
			break;
		case 'q':
			isRunning = false;
			break;
	}
}


void render(void){
	clear();
	
	get_terminal_size_pixels(&window_width,&window_height);
	getmaxyx(stdscr,terminal_height,terminal_width);
	
	float factor_x = window_width / terminal_width;
	float factor_y = window_height / terminal_height;

	//printf("%d\n",count);	
	int16_t *buf =(int16_t *)(audio_data.buffer + audio_data.position);
	size_t buf_size = 2*N;

	int h_by_2 = window_height/2;

	for(size_t i=0; i < buf_size/2; i++){
		left_sample[i] = buf[2*i];
		right_sample[i] = buf[2*i+1];
	}
	
	fft(left_sample,fft_out_left,N);
	fft(left_sample,fft_out_right,N);

	
	float max_amp = 0.0f;
	for(size_t i=0; i<N; i++){
		float c1 = cabsf(fft_out_left[i]); 
		float c2 = cabsf(fft_out_right[i]);
		float c = (c1 > c2) ? c1 : c2;	
		if(c > max_amp)
			max_amp = c;
	}
	
	int sample_rate = wav_spec.freq;
	int max_freq = sample_rate/2;

	int num_bar = window_width / 10;
	
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
		int bins_summed = 0;

		for(int j = prev_index+1 ; j <= current_index ; j++){
			if(j >=0 && j < N/2){
				float mag_left = cabsf(fft_out_left[j]);
				float mag_right = cabsf(fft_out_right[j]);
				float mag = (mag_left + mag_right)/2;
				sum_mag += mag;
				bins_summed++;
			}
		}


		if(sum_mag > 1e-4){
			bar_mag[bar_mag_i++] = sum_mag;
			//bar_mag[bar_mag_i++] = sum_mag/bins_summed; 			//taking average makes the magnitude too small for higher frequencies maybe use the maximum value or median
		}
		prev_freq = current_freq;
	}
	
	int cell_width = 0;
	if(bar_mag_i > 0)
		cell_width = window_width / bar_mag_i;
	for(int i=0; i<bar_mag_i; i++){
		float normalized = 0.0f;
		if(fabs(max_amp) > 1e-4){
			normalized = bar_mag[i] / max_amp;
			//normalized = log10f(bar_mag[i]) / log10f(max_amp);		//showing x axis also in logrithmis scale didnt look so nice
		}
		float bar_height = h_by_2 - h_by_2*normalized*0.8;
		int h = (h_by_2 - bar_height)/factor_y;
		if(h<=0){
			draw_rect(i*cell_width / factor_x  ,terminal_height/2 - 1,cell_width/factor_x,1);
		}else{
			draw_rect(i*cell_width / factor_x  ,bar_height/ factor_y ,cell_width/factor_x,(h_by_2 - bar_height)/factor_y);
		}
	}


	refresh();
}



int main(int argc, char **argv){

	if(argc < 2 ){
		fprintf(stderr,"Usage: visualizer path_to_audio_file\n");
		free_resources();
		exit(0);
	}
	
	if(SDL_Init(SDL_INIT_AUDIO) < 0){
		fprintf(stderr,"Failed to initialize SDL: %s\n",SDL_GetError());
		free_resources();
		exit(0);
	}
	isRunning = initialize_ncurses();
	setup(argv[1]);
	while(isRunning){
		process_input();
		render();
	}
		
	free_resources();	
	return 0;
}

