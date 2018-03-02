#include "Scene.h"
#include "Util.h"

void Scene::AddEntity(Entity* entity)
{
	entities.push_back(entity);
}

void Scene::AddLight(Light* light)
{
	lights.push_back(light);
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

	closestCollision.rayDirection = ray->rotation;

	return closestCollision;
}

Color Scene::CalculateLighting(CollisionResult result, Camera camera, int GISamples)
{
	const float AMBIENT_LIGHT = 0.1f;

	// strange to use a collision result, but it has normal information and will eventually have uv.  
	// Maybe I should call this something else, like a ray collision or something?

	if (!result.hit()) {
		return camera.SkyColor(result.rayDirection);
	}

	Color ambientLight = Color(AMBIENT_LIGHT, AMBIENT_LIGHT, AMBIENT_LIGHT);

	// collect ambient light using a hemisphere
	if (GISamples >= 2) {
		ambientLight = Color(0,0,0);
		for (int i = 0; i < GISamples; i ++) {
			Vec3d dir = result.normal;
			//90% sure this is not correct... need to think about this...
			dir.rotateX((randf() - 0.5) * M_PI); 
			dir.rotateY((randf() - 0.5) * M_PI); 
			Ray ray = Ray(result.location + (dir * 0.001), dir);
			CollisionResult ambientResult = Trace(&ray);
			Color ambientSample = CalculateLighting(ambientResult, camera, 0); // could use GISamples / 4, which gives multi bound lighting but is much slower 
			ambientLight = ambientLight + (ambientSample * (1.0/(float)GISamples));
		}
	}

	// first we get the color from the hit object
	Vec2d uv = result.entity->getUV(result.entity->toObjectSpace(result.location));
	Color diffuseColor = result.entity->material->getColor(uv) * result.entity->color;
	Color diffuseLight = Color();
	Color specularLight = Color();

	// next check which lights contribute to this object
	for (int i = 0; i < lights.size(); i++) 
	{

		Light* light = lights[i];
		Material* material = result.entity->material;

		// trace from light to object and see what we hit.
		float distanceToLight = (light->location - result.location).abs();
		Vec3d toLight = (light->location - result.location).normalized();

		Ray ray = Ray(result.location + (toLight * 0.001), toLight);
		
		CollisionResult shadowResult = Trace(&ray);

		if (!shadowResult.hit()) {

			// calculate diffuse contribution light from this light
			float diffusePower = Vec3d::Dot(toLight, result.normal);
			float attenuation = clipf(100.0 / distanceToLight, 0, 1);

			// calculate speculare light from this light
			Vec3d toCamera = (camera.location - result.location).normalized();
			Vec3d halfVector = (toLight + toCamera).normalized();
			float specularPower = clipf(Vec3d::Dot(result.normal, halfVector), 0, 1);
			specularPower = clipf(std::powf(specularPower, 1/material->hardness), 0, 1) * material->specularPower;

			diffuseLight = diffuseLight + light->color * diffusePower * attenuation;
			specularLight = specularLight + light->color * specularPower * attenuation;
						
		}
	}

	// Combine into one value.	
	Color col = (diffuseColor * (diffuseLight + ambientLight)) + specularLight + result.entity->material->emissiveColor;
	return col;

}
