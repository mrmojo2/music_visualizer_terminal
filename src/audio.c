#include "audio.h"
#include <stdio.h>
int count = 0;

const char *get_audio_format_name(SDL_AudioFormat format) {
    switch (format) {
        case AUDIO_U8: return "Unsigned 8-bit";
        case AUDIO_S8: return "Signed 8-bit";
        case AUDIO_U16LSB: return "Unsigned 16-bit (little-endian)";
        case AUDIO_S16LSB: return "Signed 16-bit (little-endian)";
        case AUDIO_U16MSB: return "Unsigned 16-bit (big-endian)";
        case AUDIO_S16MSB: return "Signed 16-bit (big-endian)";
        case AUDIO_S32LSB: return "Signed 32-bit (little-endian)";
        case AUDIO_S32MSB: return "Signed 32-bit (big-endian)";
        case AUDIO_F32LSB: return "32-bit float (little-endian)";
        case AUDIO_F32MSB: return "32-bit float (big-endian)";
        default: return "Unknown format";
    }
}


void audio_callback(void *user_data, unsigned char *stream, int stream_length){
	audio_t *audio_data = (audio_t *)user_data;

	memset(stream,0,stream_length);

	int remaining = audio_data->length - audio_data->position;
	int tocopy = (remaining > stream_length) ? stream_length : remaining;

	SDL_MixAudioFormat(stream,audio_data->buffer + audio_data->position,audio_data->format,tocopy,SDL_MIX_MAXVOLUME);
	audio_data->sample_buffer = stream;
	audio_data->sample_buffer_size = tocopy;
	audio_data->position += tocopy;
}
