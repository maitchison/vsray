#include "Tools.h"

namespace glm {

    /** Returns squared length of vector */
    float length2(glm::vec3 v) 
    {
        return dot(v, v);
    }

    /** Rotates vector v around the x,y, and z axis where x,y,z are in radians */
    glm::vec3 rotateXYZ(glm::vec3 v, float x, float y, float z) 
    {
        glm::vec4 vec = glm::vec4(v.x, v.y, v.z, 1);
        glm::mat4 rotationMatrix(1);
        rotationMatrix = glm::rotate(rotationMatrix, x, glm::vec3(1.0, 0.0, 0.0));
        rotationMatrix = glm::rotate(rotationMatrix, y, glm::vec3(0.0, 1.0, 0.0));
        rotationMatrix = glm::rotate(rotationMatrix, z, glm::vec3(0.0, 0.0, 1.0));
        return glm::vec3(vec * rotationMatrix);
    }


    /** Rotates vector v around the z,y, and x axis where x,y,z are in radians */
    glm::vec3 rotateZYX(glm::vec3 v, float x, float y, float z) 
    {
        glm::vec4 vec = glm::vec4(v.x, v.y, v.z, 1);
        glm::mat4 rotationMatrix(1);
        rotationMatrix = glm::rotate(rotationMatrix, z, glm::vec3(0.0, 0.0, 1.0));
        rotationMatrix = glm::rotate(rotationMatrix, y, glm::vec3(0.0, 1.0, 0.0));
        rotationMatrix = glm::rotate(rotationMatrix, x, glm::vec3(1.0, 0.0, 0.0));
        return vec3(vec * rotationMatrix);
    }

}
