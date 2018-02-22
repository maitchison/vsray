#pragma once

#include <Entity.h>
#include <vector>

class Scene
{
	std::vector<Entity*> entities = std::vector<Entity*>();

public:

	/** Adds entity to the scene. */
	void AddEntity(Entity* entity);
	
	/** Traces ray through the scene. */
	CollisionResult Trace(Ray* ray);
};
