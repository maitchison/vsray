#pragma once

#include <Color.h>
#include <stdint.h>

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
	#include <GL/gl.h>
#endif


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

class GFX
{
	
	uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
	GLuint tex;

public:
	void GFX::putPixel(int x, int y, Color col);
	void GFX::clear(Color col);
	void GFX::blit();
	void GFX::init(void);
};

// singleton access to graphics library.
extern GFX gfx;
