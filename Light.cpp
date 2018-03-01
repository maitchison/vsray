#include "Light.h"

Light::Light(Vec3d location) : Entity(location)
{
}

Light::Light(Vec3d location, Color color) : Entity(location)
{
	this->color = color;
}
