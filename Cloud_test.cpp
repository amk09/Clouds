#include "Cloud.h"
#include <cmath>
#include <algorithm>
#include <iostream>

/*
New edit: added shapeOffset attribute. 


//To use it, in the main function, after the line float densityOffset = 0.5f, densityMultiplier = 1.2f, lightAbsorption = 0.5f;
// add the following: 
// glm::vec3 shapeOffset = glm::vec3(0.f, 0.f, 0.f);


// Inside the frame loop:
// cloud.shapeOffset.x+=0.1f; // Increment time
*/
#define MAX_SDF_SPHERE_STEPS 5.f
#define MARCH_MULTIPLIER 1.8f
#define ABSORPTION_CUTOFF 0.25f
#define ABSORPTION_COEFFICIENT 0.5f
Cloud::Cloud(glm::vec3 center, float length, float breadth, float height, float densityOffset, glm::vec3 shapeOffset, float densityMultiplier, float lightAbsorption): 
      center(center),
      length(length),
      breadth(breadth),
      height(height),
      densityOffset(densityOffset),  //less density offset, denser the cloud
      shapeOffset(shapeOffset),
      densityMultiplier(densityMultiplier),
      lightAbsorption(lightAbsorption),
      numSteps(32),
      numStepsLight(16) {


      }

float fBm(glm::vec3 pos, int octaves, float lacunarity=2.f, float gain=0.f) {
    float amplitude = 1.f;
    float frequency = 1.f;
    float result = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        result += amplitude * glm::perlin(pos * frequency);
        amplitude *= gain;
        frequency *= lacunarity;
    }
    return result;
}

glm::vec3 randomVec3(glm::vec3 seed) {
    return glm::vec3(
        glm::fract(glm::sin(glm::dot(seed, glm::vec3(12.9898, 78.233, 45.164))) * 43758.5453),
        glm::fract(glm::sin(glm::dot(seed, glm::vec3(93.9898, 67.345, 12.664))) * 43758.5453),
        glm::fract(glm::sin(glm::dot(seed, glm::vec3(43.3984, 12.345, 76.233))) * 43758.5453)
    );
}


float worleyNoise(glm::vec3 pos) {
    // Define cell boundaries and loop over neighbors
    glm::vec3 cell = floor(pos);
    float minDist = 1.0;

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
                glm::vec3 neighbor = cell + glm::vec3(dx, dy, dz);
                glm::vec3 point = neighbor + randomVec3(neighbor); // Random offset
                float dist = length(pos - point);
                minDist = glm::min(minDist, dist);
            }
        }
    }
    return minDist;
}
float ridgedNoise(glm::vec3 pos, int octaves, float gain = 0.5f, float lacunarity = 2.0f) {
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float total = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        float noise = glm::perlin(pos * frequency);
        total += amplitude * (1.0f - std::abs(noise));
        amplitude *= gain;
        frequency *= lacunarity;
    }
    return total;
}


// float Cloud::sampleDensity(glm::vec3 position) const {
//     // Base shape noise
//     glm::vec3 offsetPosition = position - shapeOffset;
//     // Base shape noise using fBm for fractal detail
//     float shapeNoise = fBm(offsetPosition * 5.0f, 5, 2.0f, 0.5f);
//     // Fine detail noise using Worley noise
//     float detailNoise = worleyNoise(offsetPosition * 10.0f);
//     // Combine base and detail noise
//     float combinedNoise = glm::mix(shapeNoise, detailNoise, 0.5f);
//     // Height-dependent density modulation
//     float heightPercent = (position.y - (center.y - breadth / 2.0f)) / breadth;
//     float heightGradient = saturate(ReMap(heightPercent, 0.0f, 0.2f, 0.0f, 1.0f)) *
//                            saturate(ReMap(heightPercent, 1.0f, 0.7f, 0.0f, 1.0f));
//     // Apply height and shape modulation
//     combinedNoise *= heightGradient;
//     // Add erosion effect based on density
//     float erosion = glm::clamp(1.0f - glm::pow(shapeNoise, 2.0f), 0.0f, 1.0f);
//     combinedNoise *= erosion;
//     // Offset density for base cloud structure
//     float density = glm::max(0.0f, combinedNoise - densityOffset);
//     // Scale by density multiplier
//     density *= densityMultiplier;
//     return density;
// }


float Cloud::sampleDensity(glm::vec3 position) const 
{
    /*
    Sample Density : Given a position in the cloud, it retrieves the corresponding density
    */

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



const glm::mat3 m = glm::mat3(
    glm::vec3(1.6f, 1.2f, 0.0f),
    glm::vec3(-1.2f, 1.6f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f)
);
const glm::mat2 m2 = glm::mat2(  0.80f,  0.60f,
                      -0.60f,  0.80f );
const glm::mat3 m3  = glm::mat3( 0.00f,  0.80f,  0.60f,
                      -0.80f,  0.36f, -0.48f,
                      -0.60f, -0.48f,  0.64f );
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
    x = glm::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
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

float sdSphere( glm::vec3 p, glm::vec3 origin, float s )
{
  p -= origin;
  return glm::length(p)-s;
}
float sdPlane( glm::vec3 p )
{
	return p.y;
}
// Taken from Inigo Quilez's Rainforest ShaderToy:
// https://www.shadertoy.com/view/4ttSWf
float fbm_4( glm::vec3 x )
{   int iFrame = 0;
    float f = 2.0f;
    float s = 0.5f;
    float a = 0.0f;
    float b = 0.5f;
    for( int i= std::min(0, iFrame); i<4; i++ )
    {
        float n = noise(x);
        a += b*n;
        b *= s;
        x = f*m3*x;
    }
	return a;
}

float GetFogDensity(glm::vec3 position, float sdfDistance)
{
    const float maxSDFMultiplier = 1.0f;
    bool insideSDF = sdfDistance < 0.0f;
    float sdfMultiplier = insideSDF ? std::min(std::abs(sdfDistance), maxSDFMultiplier) : 0.0f;
 
// #if UNIFORM_FOG_DENSITY
    return sdfMultiplier;
// #else
//    return sdfMultiplier * abs(fbm_4(position / 6.0) + 0.5);
// #endif
}

glm::vec3 GetAmbientLight()
{
	return 1.2f * glm::vec3(0.03f, 0.018f, 0.018f);
}

float Luminance(glm::vec3 color)
{
    return (color.x * 0.3f) + (color.y * 0.59f) + (color.z * 0.11f);
}

bool IsColorInsignificant(glm::vec3 color)
{
    const float minValue = 0.009;
    return Luminance(color) < minValue;
}



float sdSmoothUnion( float d1, float d2, float k ) 
{
    float h = glm::clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return glm::mix( d2, d1, h ) - k*h*(1.0-h); 
}

glm::vec2 opU( glm::vec2 d1, glm::vec2 d2 )
{
	return (d1.x<d2.x) ? d1 : d2;
}

float QueryVolumetricDistanceField( glm::vec3 pos)
{    
    float iTime = 0.f;
    glm::vec3 fbmCoord = (pos + 2.0f * glm::vec3(iTime, 0.0f, iTime)) / 1.5f;
    // glm::vec3 fbmCoord = pos / 1.5f;
    float sdfValue = sdSphere(pos, glm::vec3(-8.0f, 2.0f + 20.0f * glm::sin(iTime), -1.f), 5.6f);
    sdfValue = sdSmoothUnion(sdfValue,sdSphere(pos, glm::vec3(8.0f, 8.0f + 12.0f * glm::cos(iTime), 3.f), 5.6f), 3.0f);
    sdfValue = sdSmoothUnion(sdfValue, sdSphere(pos, glm::vec3(5.0f * glm::sin(iTime), 3.0f, 0.f), 8.0f), 3.0f) + 7.0f * fbm_4(fbmCoord / 3.2f);
    // float sdfValue = sdSphere(pos, glm::vec3(-8.0, 2.0, -1.0), 5.6);
    // sdfValue = sdSmoothUnion(sdfValue, sdSphere(pos, glm::vec3(8.0, 8.0, 3.0), 5.6), 3.0f);
    // sdfValue = sdSmoothUnion(sdfValue, sdSphere(pos, glm::vec3(5.0, 3.0, 0.0), 8.0), 3.0);

    sdfValue = sdSmoothUnion(sdfValue, sdPlane(pos + glm::vec3(0.f, 0.4f, 0.f)), 22.0f);
    return sdfValue;
}

float BeerLambert(float absorption, float dist)
{
    return glm::exp(-absorption * dist);
}


//Check if the ray hits the box of the clouds(volumetric)
//return volumeDepth. 
//maxT is large number, basically check if the t is too much, that means it doesn't hit anything. If not, hit the t value, which is the result of how har you go.
float IntersectVolumetric(glm::vec3 rayOrigin, glm::vec3 rayDirection, float maxT)
{
    // Precision isn't super important, just want a decent starting point before 
    // ray marching with fixed steps
	float precis = 0.5f; 
    float t = 0.0f;
    //t is updated by the result
    for(int i=0; i<MAX_SDF_SPHERE_STEPS; i++ )
    {   
	    float result = QueryVolumetricDistanceField( rayOrigin + rayDirection*t );
        if( result < (precis) || t>maxT ) break;
        t += result;
    }
    return ( t>=maxT ) ? -1.0f : t;
}



float Cloud::lightMarch(glm::vec3 position, glm::vec3 LightPos, float radius) const 
{
    glm::vec3 dirToLight = glm::normalize(LightPos - position);
    float stepSize = glm::length(glm::vec3(length, breadth, height)) / float(numStepsLight);

    float totalDensity = 0.0f;

    #pragma omp parallel for reduction(+:totalDensity)
    for (int i = 0; i < numStepsLight; ++i) 
    {
        glm::vec3 samplePos = position + float(i) * stepSize * dirToLight;
        float density = sampleDensity(samplePos) * stepSize;
        totalDensity += density;
    }

    // Compute distance to the last sampling point (approximately representing the distance along the light ray)
    float dist = glm::distance(LightPos, position + (float)(numStepsLight - 1) * stepSize * dirToLight) - radius;

    // Compute attenuation using an inverse-square law
    // For a point light, intensity typically falls off as 1 / (distance^2).
    // Adding 1.0 to avoid division by zero at very small distances and to set a baseline intensity.
    float a = 1.0f;   // Full initial brightness
    float b = 0.14f;   // Steeper linear reduction
    float c = 0.07f;   // Stronger quadratic reduction
    float d = 0.017f;   // More pronounced exponential decay
    float distanceAttenuation = a / (b + (c * dist) + d * dist*dist);

    float transmittance = exp(-totalDensity * lightAbsorption * 1.2f) * distanceAttenuation;
    return transmittance;
}




glm::vec3 Cloud::renderClouds(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& lightPos, const glm::vec3& lightColor, const glm::vec3& backgroundColor, float radius) const 
{   
    glm::vec3 opaqueColor = glm::vec3(0.0f);
    glm::vec3 normal;
    float t;

    bool hit = true;
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
        return backgroundColor;
    }
    glm::vec3 entryPoint = rayOrigin + tMin * rayDir;
    glm::vec3 exitPoint = rayOrigin + tMax * rayDir;
    float depth = tMax - tMin;
    float volumeDepth = IntersectVolumetric(rayOrigin, rayDir, depth);
    float opaqueVisiblity = 1.0f;
    glm::vec3 volumetricColor = glm::vec3(0.0f);//all black

    if(volumeDepth > 0.f){
        //grey color
        const glm::vec3 volumeAlbedo = glm::vec3(0.8f);
        //manually define march size, now 0.6*1.8.
        const float marchSize = 0.6f * MARCH_MULTIPLIER;
        //the distance we have gone so far
        float distanceInVolume = 0.0f;
        //sDF
        float signedDistance = 0.0;
        for(int i = 0; i < numSteps; i++)
        {   
            //volumeDepth += manually defined march size, or sDF (for i = 0, marchSize)
            volumeDepth += std::max(marchSize, signedDistance);
            //if volumeDepth > depth, (tmax-tmin), or the visibility now is < 0.25 => early stop
            if(volumeDepth > depth || opaqueVisiblity < ABSORPTION_CUTOFF) break;
            //ray march to update the position. volume Depth is t. 
            glm::vec3 position = rayOrigin + volumeDepth*rayDir;
            //sdf is updated here
            signedDistance = QueryVolumetricDistanceField(position);
            if(signedDistance < 0.0f) //if sdf < 0.f
            {   
                //means we are inside the cube, and we want to keep marching. update distanceInVolume. 
                distanceInVolume += marchSize;
                //previous opaquevisibilty update
                float previousOpaqueVisiblity = opaqueVisiblity;
                //update the visibility by beerlambert function
                opaqueVisiblity *= BeerLambert(lightAbsorption * GetFogDensity(position, signedDistance), marchSize);
                //light absorption: previous opaque - after beerlambert
                float absorptionFromMarch = previousOpaqueVisiblity - opaqueVisiblity;

                glm::vec3 dirToLight = glm::normalize(lightPos - position);
                float stepSize = glm::length(glm::vec3(length, breadth, height)) / float(numStepsLight);
                //calculate the color affected by the lights. 
                float totalDensity = 0.0f;
                float dist = glm::distance(lightPos, position + (float)(numStepsLight - 1) * stepSize * dirToLight) - radius;
                float a = 1.0f;   // Full initial brightness
                float b = 0.14f;   // Steeper linear reduction
                float c = 0.07f;   // Stronger quadratic reduction
                float d = 0.017f;   // More pronounced exponential decay
                float distanceAttenuation = a / (b + (c * dist) + d * dist*dist);
                glm::vec3 lightc = lightColor * distanceAttenuation;
                if(!IsColorInsignificant(lightc)){
                    const float lightMarchSize = 0.65f * MARCH_MULTIPLIER;
                    float t = 0.0f;
                    float lightVisibility = 1.0f;
                    float signedDistance = 0.0;
                    bool stop = false;
                    #pragma omp parallel for reduction(+:lightVisibility)
                    for (int i = 0; i < numStepsLight; ++i) 
                    {          
                        if(stop) continue;
                        t+= glm::max(marchSize, signedDistance);
                        if(t > dist || lightVisibility < ABSORPTION_CUTOFF) stop = true;
                        else{
                            glm::vec3 pos = rayOrigin + t * rayDir;
                        signedDistance = QueryVolumetricDistanceField(position);
                       if(signedDistance < 0.0)
                        {
                            lightVisibility *= BeerLambert(ABSORPTION_COEFFICIENT * GetFogDensity(position, signedDistance), marchSize);
                        }
                        }
                        

                    }
                    volumetricColor += absorptionFromMarch * lightVisibility * volumeAlbedo * lightc;
                
                //after every light. absorption FromMarch 
                volumetricColor += absorptionFromMarch * volumeAlbedo * GetAmbientLight();
 
                }
                 
         }

        }



    }

    return glm::min(volumetricColor, glm::vec3(1.0f)) + opaqueVisiblity * opaqueColor;


}




// glm::vec3 Cloud::renderClouds(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& lightPos, const glm::vec3& lightColor, const glm::vec3& backgroundColor, float radius) const 
// {    

//     // glm::vec3 col = glm::vec3(0.42,0.62,1.1) - glm::vec3(0.4f * rayDir.y);
//     glm::vec3 col = backgroundColor;
//     // clouds
//     // float t = (2500.0-rayOrigin.y)/rayDir.y;
//     // if( t>0.0 )
//     // {
//     //     glm::vec3 pos = rayOrigin + t * rayDir;
//     //     glm::vec2 uv = glm::vec2(pos.x, pos.z);
//     //     float cl = fbm_9( uv * 0.00104f );
//     //     float dl = smoothstep(-0.2,0.6,cl);
//     //     col = mix( col, glm::vec3(1.0), 0.12*dl );
//     // }
    
//     // // sun glare    
//     // float sun = glm::clamp( glm::dot(kSunDir,rayDir), 0.0f, 1.0f );
//     // col +=  0.2f * glm::vec3(1.0f, 0.6f, 0.3f) * glm::pow(sun, 32.0f);
//     float sun = glm::clamp( glm::dot(kSunDir,rayDir), 0.0f, 1.0f );    
//     col = glm::vec3(0.6f,0.71f,0.75f) - rayDir.y* 0.2f *glm::vec3(1.0f,0.5f,1.0f) + 0.15f*0.5f;    
//     col += 0.2f*glm::vec3(1.0f,.6f,0.1f)* pow( sun, 8.0f );

//     //check if the ray hits the box
//     bool hit = true;

//     // Define the bounding box limits
//     glm::vec3 minBounds = center - glm::vec3(length, breadth, height) * 0.5f;
//     glm::vec3 maxBounds = center + glm::vec3(length, breadth, height) * 0.5f;

//     // Perform ray-box intersection test
//     float tMin = (minBounds.x - rayOrigin.x) / rayDir.x;
//     float tMax = (maxBounds.x - rayOrigin.x) / rayDir.x;

//     if (tMin > tMax) std::swap(tMin, tMax);

//     float tyMin = (minBounds.y - rayOrigin.y) / rayDir.y;
//     float tyMax = (maxBounds.y - rayOrigin.y) / rayDir.y;

//     if (tyMin > tyMax) std::swap(tyMin, tyMax);

//     if ((tMin > tyMax) || (tyMin > tMax)) {
//        hit= false; // No intersection, return background color

//     }

//     tMin = glm::max(tMin, tyMin);
//     tMax = glm::min(tMax, tyMax);

//     float tzMin = (minBounds.z - rayOrigin.z) / rayDir.z;
//     float tzMax = (maxBounds.z - rayOrigin.z) / rayDir.z;

//     if (tzMin > tzMax) std::swap(tzMin, tzMax);

//     if ((tMin > tzMax) || (tzMin > tMax)) {

//         hit = false; // No intersection, return background color
//     }

//     tMin = glm::max(tMin, tzMin);
//     tMax = glm::min(tMax, tzMax);

//     if (tMin < 0 && tMax < 0) {
//        hit= false; // Intersection happens behind the ray origin
//     }

//     if(hit == false){
//         return col;
//     }



//     //start ray marching.
//     // Ray intersects the box; start ray tracing within the box
//     tMin = glm::max(0.f,tMin);
//     glm::vec3 entryPoint = rayOrigin + tMin * rayDir;
//     glm::vec3 exitPoint = rayOrigin + tMax * rayDir;

//     // glm::vec3 entryPoint = center - glm::vec3(length, breadth, height) * 0.5f;
//     // glm::vec3 exitPoint = center + glm::vec3(length, breadth, height) * 0.5f;



//     float stepSize = glm::length(glm::vec3(length, breadth, height)) / float(numSteps);
//     float transmittance = 1.0f;
//     glm::vec3 lightEnergy(0.0f);
//     float dstTravelled = 0.0f;

//     //while (dstTravelled < glm::length(glm::vec3(length, breadth, height))){
//     while (dstTravelled < glm::length(entryPoint - exitPoint)) {
//         glm::vec3 rayPos = entryPoint + rayDir * dstTravelled;
//         float density = sampleDensity(rayPos);

//         if (density > 0.0f) {
//             float lightTransmittance = lightMarch(rayPos, lightPos, radius);


//             // Accumulate light energy
//             lightEnergy += density * stepSize * transmittance * lightTransmittance * lightColor;

//             // Update transmittance
//             transmittance *= exp(-density * stepSize * lightAbsorption);

//             // Early exit if transmittance is negligible
//             if (transmittance < 0.01f)
//                 break;
//         }

//         dstTravelled += stepSize;
//     }



//     glm::vec3 cloudColor = lightEnergy;
//           //agian, can repace col w. background color as before
//     glm::vec3 finalColor = col * transmittance + cloudColor;


//     return finalColor;
// }



