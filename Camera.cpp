#include "Camera.h"
#include "GFX.h"
#include "Scene.h"

Camera::Camera(Vec3d location) : Entity(location)
{
}

Color Camera::TraceRay(Ray ray, int depth)
{
	// trace the ray
	CollisionResult result = scene->Trace(&ray);
	Color col = Color(0, 0, 0);
	if (result.hit()) {

		col = scene->CalculateLighting(result, *this);

		// reflection
		// stub: add material and isReflective
		if (depth < 10) {
			Vec3d incident = (ray.rotation).normalized();
			Vec3d normal = result.normal.normalized();
			Vec3d reflected = (incident - ((normal * 2) * (Vec3d::Dot(incident, normal)))).normalized();

			// we should push off from the surface a little to make sure we don't hit the same collision point again.
			// instead I set an owner and disable self reflections

			Ray reflectionRay = Ray(result.location, reflected);
			reflectionRay.owner = result.entity;

			// trace the ray
			Color reflectedColor = TraceRay(reflectionRay, depth + 1);
			float ref = 1.0;
			float ref2 = clipf(0.5 - depth / 10, 0, 1);
			col.r = (col.r * ref) + (reflectedColor.r * (ref2));
			col.g = (col.g * ref) + (reflectedColor.g * (ref2));
			col.b = (col.b * ref) + (reflectedColor.b * (ref2));
		}
	}
	return col;
}

int Camera::Render(int pixels, int oversample, float defocus, bool autoReset)
{
	int totalPixels = SCREEN_WIDTH * SCREEN_HEIGHT;
	float aspectRatio = float(SCREEN_WIDTH / SCREEN_HEIGHT);

	if (pixels == -1) {
		pixels = totalPixels - pixelOn;
	}
	
	int i = 0;
	for (i = 0; i < pixels; i++) {

		pixelOn++;

		if (pixelOn >= totalPixels)
		{
			// reset.
			if (autoReset) pixelOn = 0;
			return i;
		}

		int x = pixelOn % SCREEN_WIDTH;
		int y = pixelOn / SCREEN_WIDTH;

		Color outputCol = Color(0, 0, 0);

		for (int j = 0; j < oversample; j++) {
			float jitterx = (oversample == 1) ? 0.5 : randf();
			float jittery = (oversample == 1) ? 0.5 : randf();

			// find the rays direction
			float rx = (2 * ((x + jitterx) / SCREEN_WIDTH) - 1) * tan(fov / 2 * M_PI / 180) * aspectRatio;
			float ry = (1 - 2 * ((y + jittery) / SCREEN_HEIGHT)) * tan(fov / 2 * M_PI / 180);
			Vec3d dir = Vec3d(rx, -ry, -1).normalized();

			dir.rotateX(rotation.x + (randf() - 0.5) * defocus);
			dir.rotateY(rotation.y + (randf() - 0.5) * defocus);
			dir.rotateZ(rotation.z);

			Ray ray = Ray(location, dir);
			Color col = TraceRay(ray);
			outputCol = outputCol + (col * (1.0/oversample));
		}
		
		gfx.putPixel(x, y, outputCol);
	}
	return i;
}