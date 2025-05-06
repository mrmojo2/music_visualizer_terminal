#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
	float x;
	float y;
} vec2_t;

float  vec2_length(vec2_t v);
vec2_t vec2_add(vec2_t, vec2_t);
vec2_t vec2_sub(vec2_t, vec2_t);
vec2_t vec2_mul(vec2_t, float);
vec2_t vec2_div(vec2_t, float);
float  vec2_dot(vec2_t, vec2_t);

#endif
