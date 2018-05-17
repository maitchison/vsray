#pragma once

/** Some extentions to the GLM library */

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace glm {

    /** Returns squared length of vector */
    float length2(glm::vec3 v);
    
    /** Rotates vector v around the x,y, and z axis where x,y,z are in radians */
    vec3 rotateXYZ(glm::vec3 v, float x, float y, float z);

    /** Rotates vector v around the z,y, and x axis where x,y,z are in radians */
    vec3 rotateZYX(glm::vec3 v, float x, float y, float z);
    
}
