#pragma once

class Scene;

#include "Entity.h"
#include "Vec3d.h"
#include "Color.h"

struct Camera : Entity
{
	Vec3d rotation = Vec3d(0, 0, 0);
	float fov = 90.0f;
	int pixelOn = 0;
	Scene* scene;

	Camera(Vec3d location);
	
	Color TraceRay(Ray* ray, int depth = 0);
	
	/** Render this number of pixels.  Rendering can be done bit by bit.  
	 @param pixels: maximum number of pixels to render.  -1 renders entire image.
	 @param oversample: number of rays to trace per pixel.  1 for standard render.
	 @param defocus: randomly defocus rays by this number of radians.  Requires high oversampling for best results.
	 @param autoReset: causes renderer to render next frame once this frame finishes rendering.
	*/
	int Render(int pixels, int oversample=1, float defocus = 0.0f, bool autoReset=false);

	Color SkyColor(Vec3d direction);
	
};
