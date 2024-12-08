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
#include "glm/gtc/noise.hpp"

class Cloud {
public:
    Cloud(glm::vec3 center, float length, float breadth, float height, float densityOffset, glm::vec3 shapeOffset, float densityMultiplier, float lightAbsorption);

    float sampleDensity(glm::vec3 position) const;
    float lightMarch(glm::vec3 position, glm::vec3 dirToLight) const;
    glm::vec3 renderClouds(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& lightDir, const glm::vec3& lightColor, const glm::vec3& backgroundColor) const;

    glm::vec3 center;
    float length, breadth, height;
    float densityOffset;
    glm::vec3 shapeOffset;
    float densityMultiplier;
    float lightAbsorption;
    int numSteps;
    int numStepsLight;
    const glm::vec3 kSunDir = glm::normalize(glm::vec3(-0.7071,0.0,-0.7071));

};

#endif // CLOUD_H

