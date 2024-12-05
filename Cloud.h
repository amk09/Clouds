#ifndef CLOUD_H
#define CLOUD_H

#include "src/rgba.h"
#include "camera.h"
#include "glm.hpp"

RGBA sphere(glm::vec4 raydir, glm::vec4 eye, glm::vec3 pos)
{
    glm::mat4 ctm = glm::translate(glm::mat4(1.0f), pos);
    glm::mat4 ctmInverse = glm::inverse(ctm);

    glm::vec4 objRayDir = ctmInverse * raydir;
    glm::vec4 objEye = ctmInverse * eye;

    glm::vec3 d = glm::vec3(objRayDir);
    glm::vec3 o = glm::vec3(objEye);

    float r = 2.f;
    float A = glm::dot(d, d);
    float B = 2.0f * glm::dot(o, d);
    float C = glm::dot(o, o) - r * r;

    float discriminant = B * B - 4 * A * C;

    if (discriminant < 0) {
        return RGBA(0,0,0,255);
    }

    float sqrtDiscriminant = glm::sqrt(discriminant);
    float t1 = (-B - sqrtDiscriminant) / (2.0f * A);
    float t2 = (-B + sqrtDiscriminant) / (2.0f * A);

    float t;
    if (t1 >= 0 && t2 >= 0)
    {
        float diameter = 2 * r;
        float value = (t2-t1)/(diameter*5);
        return RGBA(255* value,255* value,255 * value,255);
    }
    else
    {
        return RGBA(0,0,0,255);
    }

}


RGBA Color(glm::vec4 origin, glm::vec4 direction)
{
    return sphere(direction,origin,glm::vec3(0,0,-5.f));
}

#endif // CLOUD_H
