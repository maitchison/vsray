#include "Entity.h"
#include <algorithm>

//--------------------------------------------------

CollisionResult::CollisionResult(Vec3d location, Entity* entity = NULL, float distance = 0)
{
	this->location = location;
	this->entity = entity;
	this->distance = distance;
}

CollisionResult CollisionResult::Empty()
{
	return CollisionResult(Vec3d(0, 0, 0), NULL);
}

bool CollisionResult::hit()
{
	return entity != NULL;
}

//--------------------------------------------------

Entity::Entity(Vec3d location)
{
	this->location = location;
}

Vec2d Entity::getUV(Vec3d pos)
{
	// planar mapping
	return Vec2d(pos.x / 40, pos.z / 40);
}

Vec3d Entity::toObjectSpace(Vec3d pos)
{
	pos -= location;
	pos /= scale;
	pos.rotateZ(-rotation.z);
	pos.rotateY(-rotation.y);
	pos.rotateX(-rotation.x);
	return pos;
}

Vec3d Entity::toWorldSpace(Vec3d pos)
{
	pos.rotate(rotation.x, rotation.y, rotation.z);
	pos *= scale;
	pos += location;
	return pos;
}


//--------------------------------------------------

Ray::Ray(Vec3d location, Vec3d direction) 
{
	this->location = location;
	this->rotation = direction;
	this->owner = NULL;
}

Vec3d Ray::Project(Vec3d p)
{
	p = p - location;
	return location + (rotation * (Vec3d::Dot(rotation, p) / rotation.abs2()));
}

//--------------------------------------------------

Sphere::Sphere(Vec3d location, float radius) : Entity(location)
{
	this->radius = radius;
}

CollisionResult Sphere::Trace(Ray* ray) 
{
	return RaySphereIntersection(ray, this);
}

Vec2d Sphere::getUV(Vec3d pos)
{
	// spherical mapping	
	pos /= radius;
	float u = 0.5f + (atan2(pos.z, pos.x) / (M_PI * 2));
	float v = 0.5f - (asin(pos.y) / M_PI);
	return Vec2d(u, v);
}

//--------------------------------------------------

CollisionResult Plane::Trace(Ray* ray) 
{
	return RayPlaneIntersection(ray, this);
}

Plane::Plane(Vec3d location) : Entity(location)
{
}

//--------------------------------------------------

CollisionResult RaySphereIntersection(Ray* ray, Sphere* sphere)
{
	// first we project the circle center onto the line.
	// p is now the closest point of the ray to the sphere.
	Vec3d p = ray->Project(sphere->location);
	
	// now we have a triangle from the sphere's center, to the projected point, and the intesection point.
	// we use r^2 = a^2 + b^2 and solve for a.
	float b2 = (p - sphere->location).abs2();

	// check if we are too far away to hit the sphere.
	float r2 = sphere->radius * sphere->radius;
	if (b2 > r2) return CollisionResult::Empty();

	float c = sqrt(r2 - b2);

	// pick the closest point
	Vec3d d = p - ray->location;
	float l = Vec3d::Dot(ray->rotation, d) > 0 ? d.abs() : -d.abs();
	float t1 = l + c;
	float t2 = l - c;

	if (t1 > t2) std::swap(t1, t2);
	if (t1 < 0) {
		t1 = t2;
	}

	if (t1 < 0) return CollisionResult::Empty();

	Vec3d hit = ray->location + (ray->rotation * t1);

	CollisionResult result = CollisionResult(hit, sphere, t1);
	result.normal = (hit - sphere->location).normalized();
	return result;
}

CollisionResult RayPlaneIntersection(Ray* ray, Plane* plane)
{
	// translated from python from
	// https://stackoverflow.com/questions/5666222/3d-line-plane-intersection

	// rotate objects such that plane is centered at 0,0 and facing up.
	Vec3d u = ray->rotation;
	float dot = Vec3d::Dot(plane->normal, u);

	if (abs(dot) > EPSILON)
	{
		Vec3d w = ray->location - plane->location;
		float t = -Vec3d::Dot(plane->normal, w) / dot;
		if (t > EPSILON)
		{
			Vec3d hit = ray->location + ray->rotation * t;
			CollisionResult result = CollisionResult(hit, plane, t);
			result.normal = plane->normal.normalized();
			return result;
		}
	}

	// The ray is parallel to plane, or behind it.
	return CollisionResult::Empty();
}



CollisionResult Entity::Trace(Ray* ray)
{
	return CollisionResult::Empty();
}
