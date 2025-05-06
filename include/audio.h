#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <stdbool.h>

extern int count;
extern bool audio_is_playing;

typedef struct {
	uint16_t left;
	uint16_t right;
} frame;

typedef struct {
	uint8_t *buffer;	        //buffer for entire audio file
	uint32_t length;
	uint32_t position;
	SDL_AudioFormat format;
} audio_t;

const char *get_audio_format_name(SDL_AudioFormat format);
void audio_callback(void *user_data, unsigned char *stream, int stream_length);

#endif
