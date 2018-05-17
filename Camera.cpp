#include "Camera.h"
#include "GFX.h"
#include "Scene.h"

Camera::Camera(glm::vec3 location) : Entity(location)
{
}

Color Camera::SkyColor(glm::vec3 direction)
{
	float theta = direction.y;

	// some ok sky colors
	// 170 110 93 0
	// 200 190 170 0.2
	// 20 150 180 0.3
	// 20 50 80 1
	Color skyColor = Color((200.0f/256) - theta / 2, (100.0f/256) - theta / 4, (160.0f/256) + theta/5);
	return skyColor * 0.1f + Color(0.5f,0.5f,0.9f) * 0.3f;
}

Color Camera::TraceRay(Ray* ray, int depth)
{
	// trace the ray
	CollisionResult result = scene->Trace(ray);
	Color col = Color(0, 0, 0);
	if (result.hit()) {

		int GISamples = 64;
		col = scene->CalculateLighting(result, *this, GISamples);

		Material* hitMaterial = result.entity->material;
		float reflectivity = hitMaterial->reflectivity;

		// reflection
		if ((reflectivity > 0) && (depth < 4)) {
			glm::vec3 incident = glm::normalize(ray->rotation);
			glm::vec3 normal = glm::normalize(result.normal);
			glm::vec3 reflected = glm::normalize(incident - ((normal * 2.0f) * (glm::dot(incident, normal))));

			// we should push off from the surface a little to make sure we don't hit the same collision point again.
			// instead I set an owner and disable self reflections

			Ray reflectionRay = Ray(result.location, reflected);
			reflectionRay.owner = result.entity;

			float scatterRadians = hitMaterial->reflectionScatter * M_PI / 180;

			if (scatterRadians > 0) {
                // note this is not quiet correct, we should scatter in the hemisphere facing away from the normal
                // with just 2 degrees of freedom.  The results should be similar however.
				float scatterX = (randf() - 0.5) * scatterRadians;
				float scatterY = (randf() - 0.5) * scatterRadians;
				float scatterZ = (randf() - 0.5) * scatterRadians;

                reflectionRay.rotation = rotateXYZ(reflectionRay.rotation, scatterX, scatterY, scatterZ);                
			}

			// trace the ray
			Color reflectedColor = TraceRay(&reflectionRay, depth + 1);
			float orgFactor = 1.0 - reflectivity;
			float refFactor = reflectivity * clipf(0.5 - depth / 10, 0, 1);
			col.r = (col.r * orgFactor) + (reflectedColor.r * (refFactor));
			col.g = (col.g * orgFactor) + (reflectedColor.g * (refFactor));
			col.b = (col.b * orgFactor) + (reflectedColor.b * (refFactor));
		}
	} else {
		col = SkyColor(ray->rotation);
	}
	return col;
}

int block(int x, int blockSize, int width)
{
	int blocks = ((width-1) / blockSize)+1;
	int block = x % blocks;
	return block * blockSize + x / blocks;
}

int Camera::Render(int pixels, int oversample, float defocus, bool autoReset)
{

	bool PREVIEW_MODE = false;

	int totalPixels = SCREEN_WIDTH * SCREEN_HEIGHT;
	float aspectRatio = float(SCREEN_WIDTH / SCREEN_HEIGHT);

	if (pixels == -1) {
		pixels = totalPixels - pixelOn;
	}
	
	int pixelsDone = 0;

	#pragma loop(hint_parallel(4))  
	for (int i = 0; i < pixels; i++) {

		pixelOn++;		

		if (pixelOn >= totalPixels)
		{
			// reset.
			if (autoReset) pixelOn = 0;
			return i;
		}

		int x = 0;
		int y = 0;

		if (PREVIEW_MODE) {
			// render pixels in a different order.
			x = pixelOn % SCREEN_WIDTH;
			y = pixelOn / SCREEN_WIDTH;
			y = block(y, 16, SCREEN_HEIGHT);
		} else {
			// standard scan
			x = pixelOn % SCREEN_WIDTH;
			y = pixelOn / SCREEN_WIDTH;
		}

		Color outputCol = Color(0, 0, 0);

		for (int j = 0; j < oversample; j++) {
			float jitterx = (oversample == 1) ? 0.5 : randf();
			float jittery = (oversample == 1) ? 0.5 : randf();

			// find the rays direction
			float rx = (2 * ((x + jitterx) / SCREEN_WIDTH) - 1) * tan(fov / 2 * M_PI / 180) * aspectRatio;
			float ry = (1 - 2 * ((y + jittery) / SCREEN_HEIGHT)) * tan(fov / 2 * M_PI / 180);
			glm::vec3 dir = glm::normalize(glm::vec3(rx, -ry, -1));


			dir = rotateXYZ(dir, rotation.x + (randf() - 0.5) * defocus, rotation.y + (randf() - 0.5) * defocus, rotation.z);
			
			Ray ray = Ray(location, dir);
			Color col = TraceRay(&ray);
			outputCol = outputCol + (col * (1.0/oversample));
		}
		
		if (PREVIEW_MODE && (y % 16) == 0) {
			for (int k = 0; k < 16; k ++)
				gfx.putPixel(x, y+k, outputCol);		
		} else {	
			gfx.putPixel(x, y, outputCol);
			gfx.putPixel(x, y+2, Color(1,1,1));
			gfx.putPixel(x, y+1, Color(0,0,0));
			gfx.putPixel(x, y+3, Color(0,0,0));
		}

		pixelsDone++;

	}
	return pixelsDone;
}