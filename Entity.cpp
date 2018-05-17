#include "Entity.h"
#include <algorithm>

//--------------------------------------------------

CollisionResult::CollisionResult(glm::vec3 location, Entity* entity = NULL, float distance = 0)
{
	this->location = location;
	this->entity = entity;
	this->distance = distance;
}

CollisionResult CollisionResult::Empty()
{
	return CollisionResult(glm::vec3(0, 0, 0), NULL);
}

bool CollisionResult::hit()
{
	return entity != NULL;
}

//--------------------------------------------------

Entity::Entity(glm::vec3 location)
{
	this->location = location;
}

glm::vec2 Entity::getUV(glm::vec3 pos)
{
	// planar mapping
	return glm::vec2(pos.x / 40, pos.z / 40);
}

glm::vec3 Entity::toObjectSpace(glm::vec3 pos)
{
	pos -= location;
	pos /= scale;
    pos = rotateZYX(pos, -rotation.x, -rotation.y, -rotation.z);	
	return pos;
}

glm::vec3 Entity::toWorldSpace(glm::vec3 pos)
{
	pos = rotateXYZ(pos, rotation.x, rotation.y, rotation.z);
	pos *= scale;
	pos += location;
	return pos;
}


//--------------------------------------------------

Ray::Ray(glm::vec3 location, glm::vec3 direction) 
{
	this->location = location;
	this->rotation = direction;
	this->owner = NULL;
}

glm::vec3 Ray::Project(glm::vec3 p)
{
	p = p - location;
	return location + (rotation * (glm::dot(rotation, p) / glm::length2(rotation)));
}

//--------------------------------------------------

Sphere::Sphere(glm::vec3 location, float radius) : Entity(location)
{
	this->radius = radius;
}

CollisionResult Sphere::Trace(Ray* ray) 
{
	return RaySphereIntersection(ray, this);
}

glm::vec2 Sphere::getUV(glm::vec3 pos)
{
	// spherical mapping	
	pos /= radius;
	float u = 0.5f + (atan2(pos.z, pos.x) / (M_PI * 2));
	float v = 0.5f - (asin(pos.y) / M_PI);
	return glm::vec2(u, v);
}

//--------------------------------------------------

CollisionResult Plane::Trace(Ray* ray) 
{
	return RayPlaneIntersection(ray, this);
}

Plane::Plane(glm::vec3 location) : Entity(location)
{
	normal = glm::vec3(0, 1, 0);
}

//--------------------------------------------------

CollisionResult RaySphereIntersection(Ray* ray, Sphere* sphere)
{
	// first we project the circle center onto the line.
	// p is now the closest point of the ray to the sphere.
	glm::vec3 p = ray->Project(sphere->location);
	
	// now we have a triangle from the sphere's center, to the projected point, and the intesection point.
	// we use r^2 = a^2 + b^2 and solve for a.
	float b2 = glm::length2(p - sphere->location);

	// check if we are too far away to hit the sphere.
	float r2 = sphere->radius * sphere->radius;
	if (b2 > r2) return CollisionResult::Empty();

	float c = sqrt(r2 - b2);

	// pick the closest point
	glm::vec3 d = p - ray->location;
	float l = glm::dot(ray->rotation, d) > 0 ? glm::length(d) : -glm::length(d);
	float t1 = l + c;
	float t2 = l - c;

	if (t1 > t2) std::swap(t1, t2);
	if (t1 < 0) {
		t1 = t2;
	}

	if (t1 < 0) return CollisionResult::Empty();

	glm::vec3 hit = ray->location + (ray->rotation * t1);

	CollisionResult result = CollisionResult(hit, sphere, t1);
	result.normal = glm::normalize(hit - sphere->location);
	return result;
}

CollisionResult RayPlaneIntersection(Ray* ray, Plane* plane)
{
	// translated from python from
	// https://stackoverflow.com/questions/5666222/3d-line-plane-intersection

	// rotate objects such that plane is centered at 0,0 and facing up.
	glm::vec3 u = ray->rotation;
	float dot = glm::dot(plane->normal, u);

	if (std::fabs(dot) > EPSILON)
	{
		glm::vec3 w = ray->location - plane->location;
		float t = -glm::dot(plane->normal, w) / dot;
		if (t > EPSILON)
		{
			glm::vec3 hit = ray->location + ray->rotation * t;
			CollisionResult result = CollisionResult(hit, plane, t);
			result.normal = glm::normalize(plane->normal);
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
