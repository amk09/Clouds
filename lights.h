#ifndef LIGHT_H
#define LIGHT_H
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include <algorithm>

class Light {
public:
    // Class Members
    glm::vec3 pos;
    glm::vec3 emissionColor;
    float radius;

    // Constructor
    Light(const glm::vec3 &position, const glm::vec3 &color, float rad)
        : pos(position), emissionColor(color), radius(rad) {}

    glm::vec3 lightSphereWithGlow(const glm::vec4 &raydir, const glm::vec4 &eye) const {
        glm::mat4 ctm = glm::translate(glm::mat4(1.0f), pos);
        glm::mat4 ctmInverse = glm::inverse(ctm);
        glm::vec4 objRayDir = ctmInverse * raydir;
        glm::vec4 objEye = ctmInverse * eye;
        
        glm::vec3 d = glm::normalize(glm::vec3(objRayDir));
        glm::vec3 o = glm::vec3(objEye);
        
        float A = glm::dot(d, d);
        float B = 2.0f * glm::dot(o, d);
        float C = glm::dot(o, o) - radius * radius;
        float discriminant = B * B - 4 * A * C;

        // World-space ray origin and direction
        glm::vec3 rayOrigin = glm::vec3(eye);
        glm::vec3 rayDirWorld = glm::normalize(glm::vec3(raydir));

        // Glow radius and intensity calculation
        float glowRadius = radius * 3.0f;
        
        // If the ray intersects the sphere or passes near it
        if (discriminant >= 0) {
            float sqrtDiscriminant = glm::sqrt(discriminant);
            float t1 = (-B - sqrtDiscriminant) / (2.0f * A);
            float t2 = (-B + sqrtDiscriminant) / (2.0f * A);

            // If intersection points are in front of the ray origin
            if (t1 >= 0.0f || t2 >= 0.0f) {
                return emissionColor;  // Direct light emission
            }
        }

        // Calculate distance from ray to sphere center
        glm::vec3 centerToOrigin = pos - rayOrigin;
        float projection = glm::dot(centerToOrigin, rayDirWorld);
        glm::vec3 closestPoint = rayOrigin + projection * rayDirWorld;
        float distanceToCenter = glm::length(pos - closestPoint);

        // Compute glow intensity
        float glowIntensity = glm::clamp(1.0f - (distanceToCenter / glowRadius), 0.0f, 1.0f);
        return emissionColor * glowIntensity;
    }
};
#endif // LIGHT_H