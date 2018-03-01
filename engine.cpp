//#include <GL/freeglut.h>
//#include <GL/freeglut_ext.h>
//#include <GL/freeglut_std.h>

/*

todo:

[done] Scenes 
[done] Planes
[done] Reflections

[done] progessive render
[done] create graphics library class
[done] put classes into files
[done] lighting
[done] shadows
[done] Entity Transforms (rotation, scale, etc)
[done] UV mapping
[done] materials

[done] aa
[done] camera defocus

[done] blur reflections

low res preview

triangles / meshes
light gathering (indirect lighting)
cubes
clipped planes
polygons

*/

#define _USE_MATH_DEFINES

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <GL/glut.h>
	#include <GL/gl.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <stdint.h>
#include <algorithm>
#include <limits>
#include <vector>
#include <time.h>

#include "GFX.h"
#include "Color.h"
#include "Vec3d.h"
#include "Util.h"
#include "Camera.h"
#include "Scene.h"

const int SCREEN_SCALE = 1;

// Number of rays per pixel to cast in LQ mode.
const int LQ_RAYS = 1;
const int HQ_RAYS = 16;


float currentTime = 0.00;

Scene scene = Scene();

Camera camera = Camera(Vec3d());


void display(void)
{
	gfx.blit();
}

void initScene(void)
{
	camera.fov = 90;
	camera.scene = &scene;
	camera.location = Vec3d(0, 10, 20);
	camera.rotation.x = -0.5;	

	scene.AddLight(new Light(Vec3d(30, 30, 10), Color(1, 1, 1)));


	Sphere* sphere1;
	Sphere* sphere2;
	Sphere* sphere3;

	sphere1 = new Sphere(Vec3d(0, 5, 0), 5);
	sphere1->color = Color(1.0f, 0.5, 0.5);
	scene.AddEntity(sphere1);	

	sphere2 = new Sphere(Vec3d(-12, 5, +4), 5);
	sphere2->color = Color(0.5, 1.0f, 0.5);
	scene.AddEntity(sphere2);

	sphere3 = new Sphere(Vec3d(12, 3, -7), 3);
	sphere3->color = Color(0.5, 0.5, 1.0f);
	scene.AddEntity(sphere3);

	Plane* plane1 = new Plane(Vec3d(0,0,0));	
	scene.AddEntity(plane1);

	//plane1->material = new CheckBoardMaterial();
	plane1->material = new CheckBoardMaterial();
	sphere1->material = new CheckBoardMaterial();
	sphere2->material = new PerlinMaterial();

	plane1->material->reflectivity = 0.75;
	plane1->material->reflectionScatter = 0.0;

	sphere3->material->reflectivity = 0.9;
	sphere3->material->reflectionScatter = 5.0;


}

void initialize(void)
{
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	gfx.init();
	initScene();
}

float lastFrameTime = 0.0f;
int counter = 0;
int frameOn = 0;

const int RM_NONE = 0;
const int RM_LQ = 1;
const int RM_HQ = 2;

int render_mode = RM_LQ;

void update(void)
{
	currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;	
	float elapsed = currentTime - lastFrameTime;

	if (elapsed < (0.5 / 1000.0)) {
		return;
	}

	clock_t t;
	int f;
	t = clock();
	int pixelsRendered;
	switch (render_mode) {
		case RM_LQ:
			pixelsRendered = camera.Render(20 * 1000, LQ_RAYS);
			if (pixelsRendered == 0) {
				render_mode = RM_HQ;
				camera.pixelOn = 0;
			}
			break;
		case RM_HQ:
			pixelsRendered = camera.Render(20 * 100, HQ_RAYS);
			if (pixelsRendered == 0) {
				render_mode = RM_NONE;
			}
			break;
	}
	
	float timeTaken = float(clock() - t) / CLOCKS_PER_SEC;	
	float pixelsPerSecond = (timeTaken == 0) ? -1 : pixelsRendered / timeTaken;
	if (counter == 0) {
		printf("Pixels per second = %dk.\n", int(pixelsPerSecond/1000));
	}

	counter++;
	
	glutPostRedisplay();
	lastFrameTime = currentTime;

}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("VSRAY");
	initialize();
	glutDisplayFunc(display);
	glutIdleFunc(update);
	glutMainLoop();
	return 0;
}