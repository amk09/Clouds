#ifndef CLOUD_H
#define CLOUD_H

#include "src/rgba.h"
#include "camera.h"
#include "glm/glm.hpp"
#include "helper.h"
#include "lights.h"


class Cloud {
public:
    Cloud(glm::vec3 center, float length, float breadth, float height, float densityOffset, float densityMultiplier, float lightAbsorption);

    float sampleDensity(glm::vec3 position) const;
    float lightMarch(glm::vec3 position, glm::vec3 lightPos, float radius) const;
    glm::vec3 renderClouds(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& lightPos, const glm::vec3& lightColor, const glm::vec3& backgroundColor, float radius) const;

    glm::vec3 center;
    float length, breadth, height;
    float densityOffset;
    float densityMultiplier;
    float lightAbsorption;
    int numSteps;
    int numStepsLight;
};

#endif // CLOUD_H