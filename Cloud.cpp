#include "Cloud.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include "glm/gtc/noise.hpp"


// New edit: added shapeOffset attribute. 

// To use it, in the main function, after the line float densityOffset = 0.5f, densityMultiplier = 1.2f, lightAbsorption = 0.5f;
// add the following: 
// glm::vec3 shapeOffset = glm::vec3(0.f, 0.f, 0.f);


// Inside the frame loop:
// cloud.shapeOffset.x+=0.1f; // Increment time


Cloud::Cloud(glm::vec3 center, float length, float breadth, float height, float densityOffset, glm::vec3 shapeOffset, float densityMultiplier, float lightAbsorption)
    : center(center),
      length(length),
      breadth(breadth),
      height(height),
      densityOffset(densityOffset),//less density offset, denser the cloud
      shapeOffset(shapeOffset),
      densityMultiplier(densityMultiplier),
      lightAbsorption(lightAbsorption),
      numSteps(32),
      numStepsLight(16) {}




float Cloud::sampleDensity(glm::vec3 position) const 
{
    glm::vec3 uvw = (position - center+shapeOffset) / glm::vec3(length, breadth, height); // Normalize to the cloud volume
    float baseShape = 0.0f;
    baseShape = glm::min(baseShape, glm::length(uvw - glm::vec3(-0.3f, 0.2f, 0.0f)) - 0.4f); 
    baseShape = glm::min(baseShape, glm::length(uvw - glm::vec3(0.3f, -0.2f, 0.1f)) - 0.5f); // Sphere 2
    baseShape = glm::min(baseShape, glm::length(uvw - glm::vec3(0.0f, 0.1f, 0.3f)) - 0.6f); // Sphere 3

    // Add fractal noise for natural variation
    glm::vec3 noiseCoord = uvw * 5.0f;  
    float fractalNoise = glm::perlin(noiseCoord) +
                         0.3f * glm::perlin(noiseCoord * 2.0f) + // Reduce contribution of higher octaves
                         0.15f * glm::perlin(noiseCoord * 4.0f);

    // Combine base shape and noise to get the density
    float density = glm::max(0.0f, (baseShape + fractalNoise - densityOffset) * densityMultiplier);

    return density;
}


float Cloud::lightMarch(glm::vec3 position, glm::vec3 LightPos, float radius) const {
    glm::vec3 dirToLight = glm::normalize(LightPos - position);
    float stepSize = glm::length(glm::vec3(length, breadth, height)) / float(numStepsLight);

    float totalDensity = 0.0f;

    #pragma omp parallel for reduction(+:totalDensity)
    for (int i = 0; i < numStepsLight; ++i) {
        glm::vec3 samplePos = position + float(i) * stepSize * dirToLight;
        float density = sampleDensity(samplePos) * stepSize;
        totalDensity += density;
    }

    // Compute distance to the last sampling point (approximately representing the distance along the light ray)
    float dist = glm::distance(LightPos, position + (float)(numStepsLight - 1) * stepSize * dirToLight) - radius;

    // Compute attenuation using an inverse-square law
    // For a point light, intensity typically falls off as 1 / (distance^2).
    // Adding 1.0 to avoid division by zero at very small distances and to set a baseline intensity.
    float a = 5.f;
    float b = 1.f;
    float c = 1.f;
    float d = 1.f;
    float distanceAttenuation = a / (b + (c * dist) + d * dist*dist);

    float transmittance = exp(-totalDensity * lightAbsorption * 1.2f) * distanceAttenuation;

    return transmittance;
}



glm::vec3 Cloud::renderClouds(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& lightPos, const glm::vec3& lightColor, const glm::vec3& backgroundColor, float radius) const 
{   
    glm::vec3 col = backgroundColor;

    //check if the ray hits the box
    
    // Define the bounding box limits
    glm::vec3 minBounds = center - glm::vec3(length, breadth, height) * 0.5f;
    glm::vec3 maxBounds = center + glm::vec3(length, breadth, height) * 0.5f;

    // Perform ray-box intersection test
    float tMin = (minBounds.x - rayOrigin.x) / rayDir.x;
    float tMax = (maxBounds.x - rayOrigin.x) / rayDir.x;

    if (tMin > tMax) std::swap(tMin, tMax);

    float tyMin = (minBounds.y - rayOrigin.y) / rayDir.y;
    float tyMax = (maxBounds.y - rayOrigin.y) / rayDir.y;

    if (tyMin > tyMax) std::swap(tyMin, tyMax);

    if ((tMin > tyMax) || (tyMin > tMax)) {
        return col; // No intersection, return background color
    }

    tMin = glm::max(tMin, tyMin);
    tMax = glm::min(tMax, tyMax);

    float tzMin = (minBounds.z - rayOrigin.z) / rayDir.z;
    float tzMax = (maxBounds.z - rayOrigin.z) / rayDir.z;

    if (tzMin > tzMax) std::swap(tzMin, tzMax);

    if ((tMin > tzMax) || (tzMin > tMax)) {
        return col; // No intersection, return background color
    }

    tMin = glm::max(tMin, tzMin);
    tMax = glm::min(tMax, tzMax);

    if (tMin < 0 && tMax < 0) {
        return col; // Intersection happens behind the ray origin
    }



    //start ray marching.
    // Ray intersects the box; start ray tracing within the box
    glm::vec3 entryPoint = rayOrigin + tMin * rayDir;
    glm::vec3 exitPoint = rayOrigin + tMax * rayDir;

    // glm::vec3 entryPoint = center - glm::vec3(length, breadth, height) * 0.5f;
    // glm::vec3 exitPoint = center + glm::vec3(length, breadth, height) * 0.5f;



    float stepSize = glm::length(glm::vec3(length, breadth, height)) / float(numSteps);
    float transmittance = 1.0f;
    glm::vec3 lightEnergy(0.0f);
    float dstTravelled = 0.0f;

    //while (dstTravelled < glm::length(glm::vec3(length, breadth, height))){
    while (dstTravelled < glm::length(entryPoint - exitPoint)) {
        glm::vec3 rayPos = entryPoint + rayDir * dstTravelled;
        float density = sampleDensity(rayPos);

        if (density > 0.0f) {
            float lightTransmittance = lightMarch(rayPos, lightPos, radius);

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
    glm::vec3 finalColor = col * transmittance + cloudColor;

    return finalColor;
}



