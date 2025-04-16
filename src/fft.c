#include <stdio.h>
#include <math.h>
#include <complex.h>

float pi;

int main(void){
	pi = atan2f(1,1)*4;
	
	size_t n = 16;
	float samples[n];
	float complex out[n];

	//create a wave
	for(size_t i = 0; i<=n; ++i){
		float t = (float)i/n;
		samples[i] = cosf(2*pi*t*1) + sinf(2*pi*t*2);
	}


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

	//print frequencies
	for(size_t f = 0; f < n; ++f){
		printf("%02zu: %.2f, %.2f\n",f,creal(out[f]),cimag(out[f]));
	}
	return 0;
}
