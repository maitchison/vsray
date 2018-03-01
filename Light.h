#pragma once

#include "Entity.h"
#include "Vec3d.h"

struct Light : Entity
{
	Light(Vec3d Location);
	Light(Vec3d Location, Color color);
};

