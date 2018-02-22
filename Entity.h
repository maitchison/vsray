#pragma once

#include <Vec3d.h>
#include <Color.h>
#include <stdlib.h>

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

	/* Create a new collision result. */
	CollisionResult(Vec3d location, Entity* entity, float distance);

	/* Create an empty collision result. */
	static CollisionResult Empty();

	/* Returns if collision hit anything or not. */
	bool hit();

};

struct Entity
{
	Vec3d location;
	Color color;

	Entity(Vec3d location);
	
	/** test ray collision with this entity. */
	virtual CollisionResult Trace(Ray* ray);
	virtual Color getColor(Vec3d location);
};

struct Ray : Entity
{
	Vec3d direction;
	Entity* owner;

	Ray(Vec3d location, Vec3d direction);
	Vec3d Project(Vec3d p);
};

struct Sphere : Entity
{
	float radius;

	Sphere(Vec3d location, float radius);
	virtual CollisionResult Trace(Ray* ray) override;
	virtual Color getColor(Vec3d location) override;
};

struct Plane : Entity
{
	Vec3d normal;

	Plane(Vec3d location, Vec3d normal);
	
	virtual CollisionResult Trace(Ray* ray) override;	
	virtual Color getColor(Vec3d location) override;
};


CollisionResult RaySphereIntersection(Ray* ray, Sphere* sphere);
CollisionResult RayPlaneIntersection(Ray* ray, Plane* plane);
