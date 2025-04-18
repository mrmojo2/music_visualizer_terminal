#ifndef FFT_H
#define FFT_H

#include <complex.h>
#include <stddef.h>

void dft(float samples[], float complex out[], size_t n);
//void fft(float samples[], size_t stride ,float complex out[], size_t n);
void fft(const float* samples, float complex *out, size_t n);
#endif
