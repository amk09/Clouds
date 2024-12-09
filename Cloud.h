#ifndef CLOUD_H
#define CLOUD_H

#include "src/rgba.h"
#include "camera.h"
#include "glm/glm.hpp"
#include "helper.h"
#include "lights.h"


class Cloud {
public:
    Cloud(glm::vec3 center, float length, float breadth, float height, float densityOffset, glm::vec3 shapeOffset, float densityMultiplier, float lightAbsorption);

    float sampleDensity(glm::vec3 position) const;
    float lightMarch(glm::vec3 position, glm::vec3 lightPos, float radius) const;
    glm::vec3 renderClouds(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& lightPos, const glm::vec3& lightColor, const glm::vec3& backgroundColor, float radius) const;

    glm::vec3 center;
    float length, breadth, height;
    float densityOffset;
    glm::vec3 shapeOffset;
    float densityMultiplier;
    float lightAbsorption;
    int numSteps;
    int numStepsLight;
    float remap(float value, float inMin, float inMax, float outMin, float outMax) {
    // Map the value from [inMin, inMax] to [outMin, outMax]
    float t = (value - inMin) / (inMax - inMin);
    return outMin + t * (outMax - outMin);
}

const glm::mat2 m2 = glm::mat2(  0.80,  0.60,
                      -0.60,  0.80 );
// Hash function for `vec2`
float hash1(glm::vec2 p) {
    glm::vec2 q = 50.0f * glm::fract(p * 0.3183099f); 
    return glm::fract(q.x * q.y * (q.x + q.y));       // Return hash value
}
// Hash function for `float`
float hash1(float n) {
    return glm::fract(n * 17.0f * glm::fract(n * 0.3183099f)); // Return hash value
}
//noise function1(takes vec3)
float noise( glm::vec3 x )
{
    glm::vec3 p = floor(x);
    glm::vec3 w = fract(x);
    

     glm::vec3 u = w * w * w * (w * (w * 6.0f - 15.0f) + 10.0f);
    


    float n = p.x + 317.0*p.y + 157.0*p.z;
    
    float a = hash1(n+0.0);
    float b = hash1(n+1.0);
    float c = hash1(n+317.0);
    float d = hash1(n+318.0);
    float e = hash1(n+157.0);
	float f = hash1(n+158.0);
    float g = hash1(n+474.0);
    float h = hash1(n+475.0);

    float k0 =   a;
    float k1 =   b - a;
    float k2 =   c - a;
    float k3 =   e - a;
    float k4 =   a - b - c + d;
    float k5 =   a - c - e + g;
    float k6 =   a - b - e + f;
    float k7 = - a + b + c - d + e - f - g + h;

    return -1.0+2.0*(k0 + k1*u.x + k2*u.y + k3*u.z + k4*u.x*u.y + k5*u.y*u.z + k6*u.z*u.x + k7*u.x*u.y*u.z);
}
//noise function2 (takes vec2)
float noise( glm::vec2 x )
{
    glm::vec2 p = floor(x);
    glm::vec2 w = fract(x);

    glm::vec2 u = w * w * w * (w * (w * 6.0f - 15.0f) + 10.0f);
    

    float a = hash1(p+glm::vec2(0,0));
    float b = hash1(p+glm::vec2(1,0));
    float c = hash1(p+glm::vec2(0,1));
    float d = hash1(p+glm::vec2(1,1));
    
    return -1.0+2.0*(a + (b-a)*u.x + (c-a)*u.y + (a - b - c + d)*u.x*u.y);
}
//fbm function for sky rendering
float fbm_9( glm::vec2 x )
{
    float f = 1.9;
    float s = 0.55;
    float a = 0.0;
    float b = 0.5;
    for( int i=0; i<9; i++ )
    {
        float n = noise(x);
        a += b*n;
        b *= s;
        x = f* m2 *x;
    }
    
	return a;
}
//OpenGL smoothstep
float smoothstep(float edge0, float edge1, float x) {
    // Clamp x to the range [edge0, edge1]
    x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}
//OpenGL smoothstep
glm::vec3 smoothstep(float edge0, float edge1, glm::vec3 x) {
    return glm::vec3(
        smoothstep(edge0, edge1, x.x),
        smoothstep(edge0, edge1, x.y),
        smoothstep(edge0, edge1, x.z)
    );
}


     const glm::vec3 kSunDir = glm::normalize(glm::vec3(-0.7071,0.0,-0.7071));
};

#endif // CLOUD_H