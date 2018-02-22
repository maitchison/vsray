#include <Entity.h>

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



Entity::Entity(Vec3d location)
{
	this->location = location;
	this->color = Color(0.5f, 0.5f, 0.5f);
}

Color Entity::getColor(Vec3d location)
{
	return color;
}

Ray::Ray(Vec3d location, Vec3d direction) : Entity(location)
{
	this->direction = direction;
	this->owner = NULL;
}

Vec3d Ray::Project(Vec3d p)
{
	p = p - location;
	return location + (direction * (Vec3d::Dot(direction, p) / direction.abs2()));
}

Sphere::Sphere(Vec3d location, float radius) : Entity(location)
{
	this->radius = radius;
}

CollisionResult Sphere::Trace(Ray* ray) 
{
	return RaySphereIntersection(ray, this);
}

Color Sphere::getColor(Vec3d location) 
{
	return color;
	//bool cell = location.x < this->location.x;
	//return cell ? Color(1,0,0) : Color(0,0,1);
}

Plane::Plane(Vec3d location, Vec3d normal) : Entity(location)
{
	this->normal = normal;
}

CollisionResult Plane::Trace(Ray* ray) 
{
	return RayPlaneIntersection(ray, this);
}

Color Plane::getColor(Vec3d location) 
{
	bool cell = (int(location.x / 1) + int(location.z / 1)) % 2;
	return cell ? Color(0, 0, 0) : Color(0.5, 0.5, 0.5);
}


CollisionResult RaySphereIntersection(Ray* ray, Sphere* sphere)
{
	// first we project the circle center onto the line.
	// p is now the closest point of the ray to the sphere.
	Vec3d p = ray->Project(sphere->location);
	float l = (p - ray->location).abs();

	// now we have a triangle from the sphere's center, to the projected point, and the intesection point.
	// we use r^2 = a^2 + b^2 and solve for a.
	float b = (p - sphere->location).abs();

	// check if we are too far away to hit the sphere.
	if (b > sphere->radius) return CollisionResult::Empty();

	float r = sphere->radius;
	float c = sqrt(r * r - b * b);

	// pick the closest point
	float t1 = l + c;
	float t2 = l - c;

	/*
	float t = 0;
	if ((t1 > 0) && (t1 < t2)) t = t1;
	if ((t2 > 0) && (t2 < t1)) t = t2;
	*/
	float t = t2;

	if (t < EPSILON) return CollisionResult::Empty();

	Vec3d hit = ray->location + (ray->direction * t);

	CollisionResult result = CollisionResult(hit, sphere, t);
	result.normal = (hit - sphere->location).normalized();
	return result;
}

CollisionResult RayPlaneIntersection(Ray* ray, Plane* plane)
{
	// translated from python from
	// https://stackoverflow.com/questions/5666222/3d-line-plane-intersection

	// rotate objects such that plane is centered at 0,0 and facing up.
	Vec3d u = ray->direction;
	float dot = Vec3d::Dot(plane->normal, u);

	if (abs(dot) > EPSILON)
	{
		Vec3d w = ray->location - plane->location;
		float t = -Vec3d::Dot(plane->normal, w) / dot;
		if (t > EPSILON)
		{
			CollisionResult result = CollisionResult(ray->location + ray->direction * t, plane, t);
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
