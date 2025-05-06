#include <math.h>
#include "vector.h"

float vec2_length(vec2_t v){
	return sqrtf(v.x*v.x + v.y*v.y);
}
vec2_t vec2_add(vec2_t a, vec2_t b){
	vec2_t result = {
		.x = a.x+b.x,
		.y = a.y+b.y	
	};
	return result;
}
vec2_t vec2_sub(vec2_t a, vec2_t b){
	vec2_t result ={
		.x = a.x-b.x,
		.y = a.y-b.y
	};
	return result;
}
vec2_t vec2_mul(vec2_t v, float factor){
	vec2_t result = {
		.x = v.x*factor,
		.y = v.y*factor
	};
	return result;
}

vec2_t vec2_div(vec2_t v, float factor){
	vec2_t result = {
		.x = v.x/factor,
		.y = v.y/factor
	};
	return result;
}
float vec2_dot(vec2_t a, vec2_t b){
	return ((a.x * b.x) + (a.y * b.y));
}
