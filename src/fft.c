#include <assert.h>
#include <math.h>
#include <complex.h>
#include <stdlib.h>

#include "fft.h"

float pi = atan2f(1,1)*4;

void dft(float samples[],float complex out[], size_t n){
	//performing dft
	for(size_t f = 0; f<n/2; ++f){
		out[f] = 0;
		out[f+n/2] = 0;

		//for even samples
		for(size_t i = 0;  i<n ; i+=2){
			float t = (float)i/n;
			float complex v =  samples[i] * cexp(I*2*pi*f*t);
			out[f] += v;	
			out[f+n/2] += v; 
		}
		
		//for odd samples
		for(size_t i =1;  i<n ; i+=2){
			float t = (float)i/n;
			float complex v =  samples[i] * cexp(I*2*pi*f*t);
			out[f] += v;	
			out[f+n/2] -= v; 
		}
	}
}


//note: n must be power of 2 for fft
/*void fft(float samples[], size_t stride, float complex out[], size_t n){
	assert(n>0);

	if(n == 1) {
		out[0] = samples[0];
		return;
	}

	fft(samples,stride*2,out,n/2);
	fft(samples + stride, stride*2, out + n/2, n/2);

	for(size_t k = 0; k< n/2; ++k){
		float t = (float)k/n;
		float complex v = cexp(-2*I*pi*t)*out[k+n/2];
		float complex e = out[k];
		out[k] = e+v;
		out[k+n/2] = e-v;
	}
}*/

// Recursive Cooley-Tukey FFT
void fft(const float* input, float complex* output, size_t n) {
    if (n == 1) {
        output[0] = input[0] + 0.0f * I;
        return;
    }

    // Divide
    float* even = (float*)malloc(n / 2 * sizeof(float));
    float* odd  = (float*)malloc(n / 2 * sizeof(float));

    for (size_t i = 0; i < n / 2; i++) {
        even[i] = input[i * 2];
        odd[i]  = input[i * 2 + 1];
    }

    // Recursively FFT on even and odd
    float complex* even_fft = (float complex*)malloc(n / 2 * sizeof(float complex));
    float complex* odd_fft  = (float complex*)malloc(n / 2 * sizeof(float complex));

    fft(even, even_fft, n / 2);
    fft(odd, odd_fft, n / 2);

    // Combine
    for (size_t k = 0; k < n / 2; k++) {
        float complex t = cexpf(-2.0f * I * M_PI * k / n) * odd_fft[k];
        output[k]       = even_fft[k] + t;
        output[k + n/2] = even_fft[k] - t;
    }

    // Cleanup
    free(even);
    free(odd);
    free(even_fft);
    free(odd_fft);
}

