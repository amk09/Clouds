#include "Cloud.h"
#include <cmath>
#include <algorithm>
#include <iostream>

Cloud::Cloud(glm::vec3 center, float length, float breadth, float height, float densityOffset, float densityMultiplier, float lightAbsorption)
    : center(center),
      length(length),
      breadth(breadth),
      height(height),
      densityOffset(densityOffset),
      densityMultiplier(densityMultiplier),
      lightAbsorption(lightAbsorption),
      numSteps(64),
      numStepsLight(32) {}

float Cloud::sampleDensity(glm::vec3 position) const {
    glm::vec3 uvw = (position - center) / glm::vec3(length, breadth, height); // Normalize to the cloud volume
    float shapeNoise = glm::perlin(uvw); // Use Perlin noise to simulate density

    // Base density
    float baseDensity = std::max(0.0f, (shapeNoise - densityOffset) * densityMultiplier);

    // Optional detail noise
    if (baseDensity > 0.0f) {
        glm::vec3 detailPos = uvw * 0.5f;
        float detailNoise = glm::perlin(detailPos);
        baseDensity *= detailNoise;
    }

    return baseDensity;
}

float Cloud::lightMarch(glm::vec3 position, glm::vec3 dirToLight) const {
    glm::vec3 currentPos = position;
    float stepSize = glm::length(glm::vec3(length, breadth, height)) / float(numStepsLight);
    float totalDensity = 0.0f;

    for (int i = 0; i < numStepsLight; ++i) {
        totalDensity += sampleDensity(currentPos) * stepSize;
        currentPos += dirToLight * stepSize;
    }

    float transmittance = exp(-totalDensity * lightAbsorption);
    return transmittance;
}

glm::vec3 Cloud::renderClouds(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& lightDir, const glm::vec3& lightColor, const glm::vec3& backgroundColor) const {
    glm::vec3 entryPoint = center - glm::vec3(length, breadth, height) * 0.5f;
    glm::vec3 exitPoint = center + glm::vec3(length, breadth, height) * 0.5f;

    float stepSize = glm::length(glm::vec3(length, breadth, height)) / float(numSteps);
    float transmittance = 1.0f;
    glm::vec3 lightEnergy(0.0f);
    float dstTravelled = 0.0f;

    while (dstTravelled < glm::length(glm::vec3(length, breadth, height))) {
        glm::vec3 rayPos = entryPoint + rayDir * dstTravelled;
        float density = sampleDensity(rayPos);

        if (density > 0.0f) {
            float lightTransmittance = lightMarch(rayPos, glm::normalize(lightDir));

            // Accumulate light energy
            lightEnergy += density * stepSize * transmittance * lightTransmittance * lightColor;

            // Update transmittance
            transmittance *= exp(-density * stepSize * lightAbsorption);

            // Early exit if transmittance is negligible
            if (transmittance < 0.01f)
                break;
        }

        dstTravelled += stepSize;
    }

    glm::vec3 cloudColor = lightEnergy;
    glm::vec3 finalColor = backgroundColor * transmittance + cloudColor;

    return finalColor;
}

