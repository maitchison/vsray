#pragma once

#include <Entity.h>
#include <Vec3d.h>
#include <Color.h>
#include <Scene.h>

struct Camera : Entity
{
	Vec3d rotation = Vec3d(0, 0, 0);
	float fov = 90.0f;
	int pixelOn = 0;
	Scene* scene;

	Camera(Vec3d location);
	
	Color TraceRay(Ray ray, int depth = 0);
	
	/** Render this number of pixels.  Rendering can be done bit by bit.  Passing -1 causes entire frame to render. */
	int Render(int pixels = -1, bool autoReset = false);	

};
