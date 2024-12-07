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
    glm::vec4 cloudsMap(const glm::vec3& pos, float time, float& nearestDensity) const;
    glm::vec4 cloudsFbm(const glm::vec3& pos, float time) const;


    glm::vec3 center;
    float length, breadth, height;
    float densityOffset;
    float densityMultiplier;
    float lightAbsorption;
    int numSteps;
    int numStepsLight;

    glm::vec3 fog(glm::vec3 col, float t) const {
        glm::vec3 ext = glm::exp2(-t * 0.00025f * glm::vec3(1.0f, 1.5f, 4.0f));
        return col * ext + (1.0f - ext) * glm::vec3(0.55f, 0.55f, 0.58f);
    }



    const glm::vec3 kSunDir = glm::normalize(glm::vec3(-0.624695, 0.468521, -0.624695));
    const float kMaxTreeHeight = 4.8;
    const float kMaxHeight = 840.0;
};

#endif // CLOUD_H