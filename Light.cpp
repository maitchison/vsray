#include "Light.h"

Light::Light(glm::vec3 location) : Entity(location)
{
}

Light::Light(glm::vec3 location, Color color) : Entity(location)
{
	this->color = color;
}
