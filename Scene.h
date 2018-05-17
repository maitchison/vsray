#pragma once

#define _USE_MATH_DEFINES

#include <vector>
#include <cmath>

#include "Entity.h"
#include "Light.h"
#include "Camera.h"

class GICache;

class Scene
{
	std::vector<Entity*> entities = std::vector<Entity*>();
	std::vector<Light*> lights = std::vector<Light*>();
	GICache* giCache = NULL;

public:

	/** Adds entity to the scene. */
	void AddEntity(Entity* entity);
	
	/** Adds a light to the scene. */
	void AddLight(Light* light);

	/** Traces ray through the scene, returns collision information. */
	CollisionResult Trace(Ray* ray);

	/** Calculate the lighting at a given point in space. */
	Color CalculateLighting(CollisionResult result, Camera camera, int GISamples=16);

};

void createCoordinateSystem(const Vec3d & N, Vec3d & Nt, Vec3d & Nb);
