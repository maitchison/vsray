#pragma once

#include "Color.h"
#include "Vec3d.h"
#include "PerlinNoise.hpp"

/* Handles 2d materials for objects. */
class Material
{
public:
	Color col = Color(0.75, 0.85, 0.98);
	float reflectivity = 0.0;
	float reflectionScatter = 0.0;
	float hardness = 0.1;
	float specularPower = 1.0;
	
public:
	/* Get color from UV location. */
	virtual Color getColor(Vec2d uv)
	{
		return col;
	}

};

class CheckBoardMaterial : public Material
{
public:
	virtual Color getColor(Vec2d uv) override
	{
		float scale = 64;
		int x = (uv.x > 0) ? int(uv.x * scale) : int(-uv.x * scale + 1);
		int y = (uv.y > 0) ? int(uv.y * scale) : int(-uv.y * scale + 1);
		bool cell = (x+y) & 1;
		return cell ? Color(0, 0, 0) : col;
	}
};

class PerlinMaterial : public Material
{
public:

	siv::PerlinNoise noise;
	int octaves = 8;
	float scale = 10;

	PerlinMaterial() : Material()
	{
		int seed = 101;
		noise = siv::PerlinNoise(seed);
	}

	virtual Color getColor(Vec2d uv) override
	{
		return col * noise.octaveNoise0_1(uv.x * scale, uv.y * scale, 0, octaves);
	}
};

