#include "Scene.h"
#include "Util.h"

void createCoordinateSystem(Vec3d &N, Vec3d &Nt, Vec3d &Nb)
{
	if (std::fabs(N.x) > std::fabs(N.y))
		Nt = Vec3d(N.z, 0, -N.x) / sqrtf(N.x * N.x + N.z * N.z);
	else
		Nt = Vec3d(0, -N.z, N.y) / sqrtf(N.y * N.y + N.z * N.z);
	Nb = N.crossProduct(Nt);
}

Vec3d uniformSampleHemisphere(const float &r1, const float &r2)
{
	// cos(theta) = u1 = y
	// cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
	float sinTheta = sqrtf(1 - r1 * r1);
	float phi = 2 * M_PI * r2;
	float x = sinTheta * cosf(phi);
	float z = sinTheta * sinf(phi);
	return Vec3d(x, r1, z);
}

/** Cache for global illumination samples. */
// doesn't work very well.
class GICache
{

	Color* buffer;

public:

	const int CACHE_SIZE = 512;

	GICache()
	{
		buffer = new Color[CACHE_SIZE * CACHE_SIZE * (CACHE_SIZE / 10)];
	}

	int toBuffer(Vec3d location)
	{
		// stub:
		// center on a specific location
		location = location - Vec3d(12, 3, -30);
		int bx = (int)(location.x * 8) + (CACHE_SIZE / 2);
		int by = (int)(location.y * 8) + (CACHE_SIZE / 10 / 2);
		int bz = (int)(location.z * 8) + (CACHE_SIZE / 2);
		
		if ((uint32_t)bx < CACHE_SIZE && (uint32_t)bz < CACHE_SIZE && (uint32_t)by < (CACHE_SIZE / 10))
			return bx + bz * CACHE_SIZE + by * CACHE_SIZE * CACHE_SIZE;
		else
			return -1;
	}

	Color* getSample(Vec3d location, Vec3d normal)
	{
		int index = toBuffer(location);
		if (index != -1 && buffer[index].a != 0)
			return &buffer[index]; 
		else
			return NULL;
	}

	void setSample(Vec3d location, Vec3d normal, Color col)
	{
		int index = toBuffer(location);
		if (index != -1) {
			col.a = 1.0;
			buffer[index] = col;
		}
	}
};

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

		ambientLight = Color(0, 0, 0);
		bool hasSample = false;

		// check cache
		if (giCache) {
			Color* col = giCache->getSample(result.location, result.normal);
			if (col) {
				ambientLight = *col + Color(0,0.2,0.5);
				hasSample = true;
			}			
		}

		if (!hasSample) {
			Vec3d dir = result.normal;

			Vec3d N = result.normal;
			Vec3d Nd;
			Vec3d Nb;
			createCoordinateSystem(N, Nd, Nb);

			for (int i = 0; i < GISamples; i++) {				
				// put into local space of the normal
				Vec3d sample = uniformSampleHemisphere(randf(), randf());
				Vec3d dir = Nd* sample.x + N*sample.y + Nb*sample.z;
				
				Ray ray = Ray(result.location + (dir * 0.001), dir);
				CollisionResult ambientResult = Trace(&ray);
				Color ambientSample = CalculateLighting(ambientResult, camera, GISamples / 16); // could use GISamples / 4 or 0, which gives multi bound lighting but is much slower 
				ambientLight = ambientLight + (ambientSample * (1.0 / (float)GISamples));
			}
			if (giCache) {
				giCache->setSample(result.location, result.normal, ambientLight);
			}
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
			specularPower = clipf(pow(specularPower, 1/material->hardness), 0, 1) * material->specularPower;

			diffuseLight = diffuseLight + light->color * diffusePower * attenuation;
			specularLight = specularLight + light->color * specularPower * attenuation;
						
		}
	}

	// Combine into one value.	
	Color col = (diffuseColor * (diffuseLight + ambientLight)) + specularLight + result.entity->material->emissiveColor;
	return col;

}
