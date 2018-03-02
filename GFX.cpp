/*
Simple Graphics Library
Author Matthew Aitchison
Date Feb 2018
*/

#include "GFX.h"

/** Place a pixel on the frame buffer. */
void GFX::putPixel(int x, int y, Color col)
{
	if ((x < 0) | (y < 0) | (x >= SCREEN_WIDTH) | (y >= SCREEN_HEIGHT)) {
		return;
	}
	buffer[y][x] = col.int24();
}

void GFX::clear(Color col)
{
	uint32_t c = col.int24();
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			buffer[y][x] = c;
		}
	}
}

/** Blit buffer to screen */
void GFX::blit()
{
	//upload to GPU texture (slow, shoud use glTextSubImage2D
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	//match projection to window resolution (could be in reshape callback)
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//clear and draw quad with texture (could be in display callback)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, tex);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(0, 0);
	glTexCoord2i(0, 1); glVertex2i(0, SCREEN_HEIGHT);
	glTexCoord2i(1, 1); glVertex2i(SCREEN_WIDTH, SCREEN_HEIGHT);
	glTexCoord2i(1, 0); glVertex2i(SCREEN_WIDTH, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFlush(); //don't need this with GLUT_DOUBLE and glutSwapBuffers

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void GFX::init(void)
{
	// init texture	
	for (int y = 0; y < SCREEN_HEIGHT; y++) {
		for (int x = 0; x < SCREEN_WIDTH; x++) {
			int cell = ((x / 8) + (y / 8)) % 2;
			float v = 0.5f + cell * 0.5f;
			buffer[y][x] = Color(v, v, v).int24();
		}
	}
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
	glBindTexture(GL_TEXTURE_2D, 0);

}

// singleton GFX access;
GFX gfx = GFX();