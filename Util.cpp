#include "Util.h"

/***
* Convert from degrees to radians.
* @param: deg: the degrees to convert
* @return: the result in radians.
*/
float deg2rad(float deg)
{
	return deg * (M_PI / 180.0);
}

float randf()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

int clip(int x, int a, int b)
{
	if (x < a) return a;
	if (x > b) return b;
	return x;
}

float clipf(float x, float a, float b)
{
	if (x < a) return a;
	if (x > b) return b;
	return x;
}

float minf(float a, float b)
{
	return (a < b) ? a : b;
}

