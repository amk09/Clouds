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

    // Member function for computing the glow effect
    glm::vec3 lightSphereWithGlow(const glm::vec4 &raydir, const glm::vec4 &eye) const
    {
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

        // Define a glow radius larger than the sphere itself
        float glowRadius = radius * 3.0f;

        auto computeGlowIntensity = [&](float distanceToSphere) {
            // Linear fade from 1.0 at distance=0 to 0.0 at distance=glowRadius
            float glowIntensity = 1.0f - (distanceToSphere / glowRadius);
            // Clamp to [0, 1]
            return glm::clamp(glowIntensity, 0.0f, 1.0f);
        };

        if (discriminant < 0)
        {
            // No intersection with the sphere surface.
            // Compute how close the ray passes to the sphere center.
            glm::vec3 centerToOrigin = pos - rayOrigin;
            float projection = glm::dot(centerToOrigin, rayDirWorld);
            glm::vec3 closestPoint = rayOrigin + projection * rayDirWorld;
            float distanceToCenter = glm::length(pos - closestPoint);

            // Compute and apply glow intensity
            float glowIntensity = computeGlowIntensity(distanceToCenter);
            return emissionColor * glowIntensity;
        }

        float sqrtDiscriminant = glm::sqrt(discriminant);
        float t1 = (-B - sqrtDiscriminant) / (2.0f * A);
        float t2 = (-B + sqrtDiscriminant) / (2.0f * A);

        // If the ray truly intersects the sphere
        if (t1 >= 0.0f && t2 >= 0.0f)
        {
            // At intersection, the sphere is bright (like a light bulb core)
            return emissionColor * 2.0f;
        }
        else
        {
            // Ray grazes or misses the sphere - calculate glow again
            glm::vec3 centerToOrigin = pos - rayOrigin;
            float projection = glm::dot(centerToOrigin, rayDirWorld);
            glm::vec3 closestPoint = rayOrigin + projection * rayDirWorld;
            float distanceToCenter = glm::length(pos - closestPoint);

            float glowIntensity = computeGlowIntensity(distanceToCenter);
            return emissionColor * glowIntensity;
        }
    }
};

#endif // LIGHT_H