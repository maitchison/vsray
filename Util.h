/*
--------------------------------------------------------
Utilities
--------------------------------------------------------
*/

#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <stdio.h>
#include <stdlib.h>


float deg2rad(float deg);
float randf();
int clip(int x, int a, int b);
float clipf(float x, float a, float b);
float minf(float a, float b);