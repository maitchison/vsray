#include <Scene.h>

void Scene::AddEntity(Entity* entity)
{
	entities.push_back(entity);
}

CollisionResult Scene::Trace(Ray* ray)
{
	// we need to loop through all entities and check which one the ray hits first.
	// this can be a little slow, and it would be better to do some kind of axis-aligned
	// bounding box, or bounding sphere.

	CollisionResult closestCollision = CollisionResult::Empty();
	closestCollision.distance = 999999.0f;

	for (int i = 0; i < entities.size(); i++)
	{

		if (entities[i] == ray->owner) {
			continue;
		}

		CollisionResult result = entities[i]->Trace(ray);
		if (result.hit() && (result.distance < closestCollision.distance))
		{
			//printf("%d %f %d\n", i, result.distance, entities.size());
			closestCollision = result;
		}
	}

	return closestCollision;
}
