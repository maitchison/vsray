#pragma once

#include <stdlib.h>

#include "Vec3d.h"
#include "Color.h"
#include "Material.h"

struct CollisionResult;
struct Entity;
struct Ray;
struct Plane;
struct Sphere;

struct CollisionResult
{
	Vec3d location;
	float distance;
	Vec3d normal;
	Entity* entity;
	Vec2d uv;
	Vec3d rayDirection;

	/* Create a new collision result. */
	CollisionResult(Vec3d location, Entity* entity, float distance);

	/* Create an empty collision result. */
	static CollisionResult Empty();

	/* Returns if collision hit anything or not. */
	bool hit();

};

struct Entity
{
	Vec3d location = Vec3d();
	Vec3d rotation = Vec3d();
	Vec3d scale = Vec3d(1, 1, 1);
	Color color = Color(1, 1, 1);
	Material* material = new Material();

	Entity(Vec3d location);
	
	/** test ray collision with this entity. */
	virtual CollisionResult Trace(Ray* ray);
	Vec3d toObjectSpace(Vec3d pos);
	Vec3d toWorldSpace(Vec3d pos);

	/** get UV co-ords, position is in object space. */
	virtual Vec2d getUV(Vec3d pos);

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
	Vec3d location;
	Vec3d rotation;
	Entity* owner;

	Ray(Vec3d location, Vec3d rotation);
	Vec3d Project(Vec3d p);
};

struct Sphere : Entity
{
	float radius;

	Sphere(Vec3d location, float radius);
	virtual CollisionResult Trace(Ray* ray) override;
	virtual Vec2d getUV(Vec3d pos) override;
};

struct Plane : Entity
{	
	Vec3d normal;
	Plane(Vec3d location);
	virtual CollisionResult Trace(Ray* ray) override;		
};


CollisionResult RaySphereIntersection(Ray* ray, Sphere* sphere);
CollisionResult RayPlaneIntersection(Ray* ray, Plane* plane);
