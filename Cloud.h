// #ifndef CLOUD_H
// #define CLOUD_H

// #include "src/rgba.h"
// #include "camera.h"
// #include "glm/glm.hpp"

// RGBA raymarching(glm::vec4 raydir, glm::vec4 eye, float t1, float t2)
// {
//     int x = 0, y = 0, z = 0;
//     float step = 0.1f;

//     // Parallel loop with reduction for independent accumulation
//     #pragma omp parallel for reduction(+:x, y, z)
//     for (int i = 0; i < static_cast<int>((t2 - t1) / step); i++)
//     {
//         float t = t1 + i * step;

//         // Simulate some operations
//         x++;
//         y++;
//         z++;
//     }

//     // Clamp values to 255
//     x = std::min(x, 255);
//     y = std::min(y, 255);
//     z = std::min(z, 255);

//     return RGBA(x, y, z);
// }

// RGBA sphere(glm::vec4 raydir, glm::vec4 eye, glm::vec3 pos)
// {
//     glm::mat4 ctm = glm::translate(glm::mat4(1.0f), pos);
//     glm::mat4 ctmInverse = glm::inverse(ctm);

//     glm::vec4 objRayDir = ctmInverse * raydir;
//     glm::vec4 objEye = ctmInverse * eye;

//     glm::vec3 d = glm::vec3(objRayDir);
//     glm::vec3 o = glm::vec3(objEye);

//     float r = 2.f;
//     float A = glm::dot(d, d);
//     float B = 2.0f * glm::dot(o, d);
//     float C = glm::dot(o, o) - r * r;

//     float discriminant = B * B - 4 * A * C;

//     if (discriminant < 0) {
//         return RGBA(0,0,0,255);
//     }

//     float sqrtDiscriminant = glm::sqrt(discriminant);
//     float t1 = (-B - sqrtDiscriminant) / (2.0f * A);
//     float t2 = (-B + sqrtDiscriminant) / (2.0f * A);

//     float t;
//     if (t1 >= 0 && t2 >= 0)
//     {
//         // float diameter = 2 * r;
//         // float value = (t2-t1)/(diameter*5);
//         // return RGBA(255* value,255* value,255 * value,255);
//         return raymarching(raydir,eye,t1,t2);
//     }
//     else
//     {
//         return RGBA(0,0,0,255);
//     }

// }


// RGBA Color(glm::vec4 origin, glm::vec4 direction)
// {
//     RGBA x = sphere(direction,origin,glm::vec3(0,0,-10.f));
//     RGBA y = sphere(direction,origin,glm::vec3(5,5,-20.f));

//     return RGBA(x.r + y.r, x.g + y.g, x.b + y.b);
// }

// #endif // CLOUD_H



#ifndef CLOUD_H
#define CLOUD_H

#include "src/rgba.h"
#include "camera.h"
#include "glm/glm.hpp"
#include "helper.h"
#include "lights.h"


// // Updated raymarching function to return glm::vec3
// glm::vec3 raymarching(glm::vec4 raydir, glm::vec4 eye, float t1, float t2)
// {
//     int x = 0, y = 0, z = 0;
//     float step = 0.1f;

//     // Parallel loop with reduction for independent accumulation
//     #pragma omp parallel for reduction(+:x, y, z)
//     for (int i = 0; i < static_cast<int>((t2 - t1) / step); i++)
//     {
//         float t = t1 + i * step;

//         // Simulate some operations
//         x++;
//         y++;
//         z++;
//     }

//     // Clamp values to 255
//     x = std::min(x, 255);
//     y = std::min(y, 255);
//     z = std::min(z, 255);

//     // Return as glm::vec3 (normalize to range [0, 1])
//     return glm::vec3(x / 255.0f, y / 255.0f, z / 255.0f);
// }

// // Updated sphere function to return glm::vec3
// glm::vec3 sphere(glm::vec4 raydir, glm::vec4 eye, glm::vec3 pos)
// {
//     glm::mat4 ctm = glm::translate(glm::mat4(1.0f), pos);
//     glm::mat4 ctmInverse = glm::inverse(ctm);

//     glm::vec4 objRayDir = ctmInverse * raydir;
//     glm::vec4 objEye = ctmInverse * eye;

//     glm::vec3 d = glm::vec3(objRayDir);
//     glm::vec3 o = glm::vec3(objEye);

//     float r = 2.f;
//     float A = glm::dot(d, d);
//     float B = 2.0f * glm::dot(o, d);
//     float C = glm::dot(o, o) - r * r;

//     float discriminant = B * B - 4 * A * C;

//     if (discriminant < 0)
//     {
//         return glm::vec3(0.0f, 0.0f, 0.0f); // Return black for no intersection
//     }

//     float sqrtDiscriminant = glm::sqrt(discriminant);
//     float t1 = (-B - sqrtDiscriminant) / (2.0f * A);
//     float t2 = (-B + sqrtDiscriminant) / (2.0f * A);

//     if (t1 >= 0 && t2 >= 0)
//     {
//         return raymarching(raydir, eye, t1, t2);
//     }
//     else
//     {
//         return glm::vec3(0.0f, 0.0f, 0.0f); // Return black for no valid t
//     }
// }


// // Updated Color function to use convertVec3RGBA
// RGBA Color(glm::vec4 origin, glm::vec4 direction)
// {
//     Light lights(glm::vec3(-1, 1, -10.f), glm::vec3(1.0f,1.0f,1.0f), 0.2f);
//     Light lights2(glm::vec3(-1,0,-20.f),glm::vec3(1.0f,0.0f,0.0f), 0.2f);
//     glm::vec3 sphere1Color = sphere(direction, origin, glm::vec3(0, 0, -10.f));
//     glm::vec3 sphere2Color = sphere(direction, origin, glm::vec3(5, 5, -20.f));
//     glm::vec3 light = lights.lightSphereWithGlow(direction, origin);

//     glm::vec3 combinedColor = sphere1Color + sphere2Color + light + lights2.lightSphereWithGlow(direction,origin);

//     return convertVec3RGBA(combinedColor);
// }
// #include "glm/gtc/noise.hpp"

class Cloud {
public:
    Cloud(glm::vec3 center, float length, float breadth, float height, float densityOffset, float densityMultiplier, float lightAbsorption);

    float sampleDensity(glm::vec3 position) const;
    float lightMarch(glm::vec3 position, glm::vec3 dirToLight) const;
    glm::vec3 renderClouds(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& lightDir, const glm::vec3& lightColor, const glm::vec3& backgroundColor) const;

    glm::vec3 center;
    float length, breadth, height;
    float densityOffset;
    float densityMultiplier;
    float lightAbsorption;
    int numSteps;
    int numStepsLight;
};

#endif // CLOUD_H