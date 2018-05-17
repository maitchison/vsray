#pragma once

#include <stdlib.h>
#include <glm/glm.hpp>

#include "Tools.h"
#include "Color.h"
#include "Material.h"

struct CollisionResult;
struct Entity;
struct Ray;
struct Plane;
struct Sphere;

struct CollisionResult
{
	glm::vec3 location;
	float distance;
	glm::vec3 normal;
	Entity* entity;
	glm::vec2 uv;
	glm::vec3 rayDirection;

	/* Create a new collision result. */
	CollisionResult(glm::vec3 location, Entity* entity, float distance);

	/* Create an empty collision result. */
	static CollisionResult Empty();

	/* Returns if collision hit anything or not. */
	bool hit();

};

struct Entity
{
	glm::vec3 location = glm::vec3();
	glm::vec3 rotation = glm::vec3();
	glm::vec3 scale = glm::vec3(1, 1, 1);
	Color color = Color(1, 1, 1);
	Material* material = new Material();

	Entity(glm::vec3 location);
	
	/** test ray collision with this entity. */
	virtual CollisionResult Trace(Ray* ray);
	glm::vec3 toObjectSpace(glm::vec3 pos);
	glm::vec3 toWorldSpace(glm::vec3 pos);

	/** get UV co-ords, position is in object space. */
	virtual glm::vec2 getUV(glm::vec3 pos);

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

};

struct Ray 
{
	glm::vec3 location;
	glm::vec3 rotation;
	Entity* owner;

	Ray(glm::vec3 location, glm::vec3 rotation);
	glm::vec3 Project(glm::vec3 p);
};

struct Sphere : Entity
{
	float radius;

	Sphere(glm::vec3 location, float radius);
	virtual CollisionResult Trace(Ray* ray) override;
	virtual glm::vec2 getUV(glm::vec3 pos) override;
};

struct Plane : Entity
{	
	glm::vec3 normal;
	Plane(glm::vec3 location);
	virtual CollisionResult Trace(Ray* ray) override;		
};


CollisionResult RaySphereIntersection(Ray* ray, Sphere* sphere);
CollisionResult RayPlaneIntersection(Ray* ray, Plane* plane);
