#include "Cloud.h"
#include <cmath>
#include <algorithm>
#include <iostream>

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

float remap(float value, float inMin, float inMax, float outMin, float outMax) {
    // Map the value from [inMin, inMax] to [outMin, outMax]
    float t = (value - inMin) / (inMax - inMin);
    return outMin + t * (outMax - outMin);
}
// float Cloud::sampleDensity(glm::vec3 position) const {
//     glm::vec3 size = glm::vec3(length, breadth, height);
//     glm::vec3 uvw = (size * .5f + position); // Normalize to the cloud volume
//     glm::vec3 shapePos = uvw + shapeOffset;

//     float heightP = (position.y - (center.y - breadth / 2.0f)) / height;

//     float heightG = glm::clamp(remap(heightP, 0.0f, 0.2f, 0.0f, 1.0f), 0.0f, 1.0f) *
//                 glm::clamp(remap(heightP, 1.0f, 0.7f, 0.0f, 1.0f), 0.0f, 1.0f);

    
//     glm::vec4 shapeNoise = glm::vec4(glm::perlin(shapePos));
//     glm::vec3 normalizedShapeWeights = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)); // Placeholder weights
//     float shapeFBM = glm::dot(glm::vec3(shapeNoise), normalizedShapeWeights) * heightG;
// float normalizedFBM = (shapeFBM + 1.0f) * 0.5f; // Remap to [0, 1]
// float baseShapeDensity = normalizedFBM + densityOffset * -1.0f;


//      std::cout << "Base Shape Density: " << baseShapeDensity << std::endl;


//     // Add fractal noise for natural variation
//     if (baseShapeDensity > 0.0f) {
//         glm::vec3 detailSamplePos = uvw * glm::vec3(1.0f); // Add appropriate scaling here
//         glm::vec4 detailNoise = glm::vec4(glm::perlin(detailSamplePos)); // Assuming a 4D noise implementation
//         glm::vec3 detailWeights = glm::vec3(1.0f); // Placeholder weights
//         glm::vec3 normalizedDetailWeights = glm::normalize(detailWeights);
//         float detailFBM = glm::dot(glm::vec3(detailNoise), normalizedDetailWeights);
//         // Subtract detail noise from base shape
//         float detailErodeWeight = (1.0f - shapeFBM) * (1.0f - shapeFBM);
//         float cloudDensity = baseShapeDensity * (1.0f - detailFBM) * detailErodeWeight;

//         return cloudDensity * densityMultiplier * 0.1f;
//     }

//     return 0.f;
// }


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




float Cloud::lightMarch(glm::vec3 position, glm::vec3 dirToLight) const {
    glm::vec3 currentPos = position;
    float stepSize = glm::length(glm::vec3(length, breadth, height)) / float(numStepsLight);
    float totalDensity = 0.0f;

    for (int i = 0; i < numStepsLight; ++i) {
        totalDensity += sampleDensity(currentPos) * stepSize;
        currentPos += dirToLight * stepSize;

        if (exp(-totalDensity * lightAbsorption) < 0.01f)
            break;
    }

    float transmittance = exp(-totalDensity * lightAbsorption * 1.2f);
    return transmittance;
}
const glm::mat2 m2 = glm::mat2(  0.80,  0.60,
                      -0.60,  0.80 );

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

glm::vec3 Cloud::renderClouds(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& lightDir, const glm::vec3& lightColor, const glm::vec3& backgroundColor) const {
    glm::vec3 col = glm::vec3(0.42,0.62,1.1) - glm::vec3(0.4f * rayDir.y);

        // clouds
        float t = (2500.0-rayOrigin.y)/rayDir.y;
        if( t>0.0 )
        {
            glm::vec3 pos = rayOrigin + t * rayDir;
            glm::vec2 uv = glm::vec2(pos.x, pos.z);
            float cl = fbm_9( uv * 0.00104f );
            float dl = smoothstep(-0.2,0.6,cl);
            col = mix( col, glm::vec3(1.0), 0.12*dl );
        }
        
        // sun glare    
        float sun = glm::clamp( glm::dot(kSunDir,rayDir), 0.0f, 1.0f );
        col +=  0.2f * glm::vec3(1.0f, 0.6f, 0.3f) * glm::pow(sun, 32.0f);

        // float sun = glm::clamp( glm::dot(kSunDir,rayDir), 0.0f, 1.0f );

        // glm::vec3 col = glm::vec3(0.76f,0.75f,0.95f)*-0.001f;

        // col -= 0.8f*glm::vec3(0.90f*rayDir.y,0.75f*rayDir.y,0.95f*rayDir.y);

        // col +=  0.2f * glm::vec3(1.0f, 0.6f, 0.1f) * glm::pow(sun, 8.0f);


        // col += 0.2f*glm::vec3(1.0f,0.4f,0.2f) * glm::pow(sun,3.0f);
        // col = smoothstep(0.15f, 1.1f, col);
    
    
    //check if the ray hits the box
    bool hit = true;
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
       hit= false; // No intersection, return background color
    }

    tMin = glm::max(tMin, tyMin);
    tMax = glm::min(tMax, tyMax);

    float tzMin = (minBounds.z - rayOrigin.z) / rayDir.z;
    float tzMax = (maxBounds.z - rayOrigin.z) / rayDir.z;

    if (tzMin > tzMax) std::swap(tzMin, tzMax);

    if ((tMin > tzMax) || (tzMin > tMax)) {
        hit = false; // No intersection, return background color
    }

    tMin = glm::max(tMin, tzMin);
    tMax = glm::min(tMax, tzMax);

    if (tMin < 0 && tMax < 0) {
       hit= false; // Intersection happens behind the ray origin
    }

    if(hit == false){

        return col;
    }






    //start ray marching.
    // Ray intersects the box; start ray tracing within the box
    glm::vec3 entryPoint = rayOrigin + tMin * rayDir;
    glm::vec3 exitPoint = rayOrigin + tMax * rayDir;



    //  glm::vec3 entryPoint = center - glm::vec3(length, breadth, height) * 0.5f;
    // glm::vec3 exitPoint = center + glm::vec3(length, breadth, height) * 0.5f;


    float stepSize = glm::length(glm::vec3(length, breadth, height)) / float(numSteps);
    float transmittance = 1.0f;
    glm::vec3 lightEnergy(0.0f);
    float dstTravelled = 0.0f;

    while (dstTravelled < glm::length(entryPoint - exitPoint)) {
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
          
    glm::vec3 finalColor = col * transmittance + cloudColor;

    return finalColor;
}
