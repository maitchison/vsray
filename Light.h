#pragma once

#include <glm/glm.hpp>

#include "Entity.h"

struct Light : Entity
{
	Light(glm::vec3 Location);
	Light(glm::vec3 Location, Color color);
};

