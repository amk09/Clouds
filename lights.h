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




// Implicit surface: x^2 + z^2 - r^2 = 0 (circular paraboloid)
// Returns the intersection parameter t and color
inline glm::vec3 implicitPlaneIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDir, float y, const glm::vec3& lightPos, const glm::vec3& lightColor, const glm::vec3& surfaceBaseColor) {
    // Check if the ray is parallel to the plane
    if (glm::abs(rayDir.y) < 1e-6) {
        // No intersection, return black (no color)
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // Compute the t value for the intersection with the plane
    float t = (y - rayOrigin.y) / rayDir.y;

    // If t is negative, the intersection is behind the ray origin
    if (t < 0.0f) {
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // Compute the intersection point on the plane
    glm::vec3 intersectionPoint = rayOrigin + t * rayDir;

    // Compute the distance between the light and the intersection point
    float distToLight = glm::length(lightPos - intersectionPoint);

    // Light attenuation using an inverse-square law
    float attenuation = glm::clamp(1.0f / (1.0f + distToLight * distToLight / (10.f * 10.f)), 0.0f, 1.0f);

    // Compute the light direction
    glm::vec3 dirToLight = glm::normalize(lightPos - intersectionPoint);

    // Surface normal (upward for the plane)
    glm::vec3 normal(0.0f, 1.0f, 0.0f);

    // Compute diffuse lighting (Lambertian shading)
    float diffuse = glm::clamp(glm::dot(normal, dirToLight), 0.0f, 1.0f);

    // Combine base color, light color, and attenuation
    glm::vec3 finalColor = surfaceBaseColor * lightColor * diffuse * attenuation;

    return finalColor;
}


inline glm::vec3 implicitWavySurfaceIntersect(const glm::vec3& rayOrigin, const glm::vec3& rayDir, float y, const glm::vec3& lightPos, const glm::vec3& lightColor, const glm::vec3& surfaceBaseColor) {
    float amplitude = 1.0f;                 // Amplitude of the waves
    float frequency = 2.0f;  
    // Check if the ray is parallel to the wavy surface
    if (glm::abs(rayDir.y) < 1e-6) {
        // No intersection, return black (no color)
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // Define the implicit wavy surface height as a function of x and z
    auto surfaceHeight = [&](float x, float z) {
        return amplitude * glm::sin(frequency * x) * glm::cos(frequency * z);
    };

    // Perform ray marching to find the intersection with the surface
    float t = 0.0f;
    const float tStep = 0.01f; // Small step size for ray marching
    const float maxT = 100.0f; // Limit to prevent infinite loops
    glm::vec3 intersectionPoint;

    while (t < maxT) {
        intersectionPoint = rayOrigin + t * rayDir;
        float ySurface = surfaceHeight(intersectionPoint.x, intersectionPoint.z);

        // Check if the ray is close enough to the surface
        if (glm::abs(intersectionPoint.y - ySurface) < 1e-3) {
            break;
        }

        // Increment the ray marching step
        t += tStep;
    }

    // If no intersection is found within maxT, return black
    if (t >= maxT) {
        return glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // Compute the distance between the light and the intersection point
    float distToLight = glm::length(lightPos - intersectionPoint);

    // Light attenuation using an inverse-square law
    float attenuation = glm::clamp(1.0f / (1.0f + distToLight * distToLight / (10.f * 10.f)), 0.0f, 1.0f);

    // Compute the gradient of the surface for the normal
    glm::vec3 gradient(
        -amplitude * frequency * glm::cos(frequency * intersectionPoint.x) * glm::cos(frequency * intersectionPoint.z), // Partial derivative w.r.t. x
        1.0f, // Partial derivative w.r.t. y
        -amplitude * frequency * glm::sin(frequency * intersectionPoint.x) * glm::sin(frequency * intersectionPoint.z)  // Partial derivative w.r.t. z
    );
    glm::vec3 normal = glm::normalize(gradient);

    // Compute the light direction
    glm::vec3 dirToLight = glm::normalize(lightPos - intersectionPoint);

    // Compute diffuse lighting (Lambertian shading)
    float diffuse = glm::clamp(glm::dot(normal, dirToLight), 0.0f, 1.0f);

    // Combine base color, light color, and attenuation
    glm::vec3 finalColor = surfaceBaseColor * lightColor * diffuse * attenuation;

    return finalColor;
}


#endif // LIGHT_H