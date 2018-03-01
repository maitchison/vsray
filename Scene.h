#pragma once

#include <vector>

#include "Entity.h"
#include "Light.h"
#include "Camera.h"

class Scene
{
	std::vector<Entity*> entities = std::vector<Entity*>();
	std::vector<Light*> lights = std::vector<Light*>();

public:

	/** Adds entity to the scene. */
	void AddEntity(Entity* entity);
	
	/** Adds a light to the scene. */
	void AddLight(Light* light);

	/** Traces ray through the scene, returns collision information. */
	CollisionResult Trace(Ray* ray);

	/** Calculate the lighting at a given point in space. */
	Color CalculateLighting(CollisionResult result, Camera camera);

};
