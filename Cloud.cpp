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
      numStepsLight(16) {}



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
    glm::vec3 noiseCoord = uvw * 5.0f;   // change here to 5.0f
    float fractalNoise = glm::perlin(noiseCoord) +
                         0.3f * glm::perlin(noiseCoord * 2.0f) + // Reduce contribution of higher octaves
                         0.15f * glm::perlin(noiseCoord * 4.0f);

    // Combine base shape and noise to get the density
    float density = glm::max(0.0f, (baseShape + fractalNoise - densityOffset) * densityMultiplier);
    return density;
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
    float a = 1.0f;   // Moderate initial intensity
    float b = 0.14f;  // Slight linear reduction
    float c = 0.07f;  // Mild quadratic reduction
    float d = 0.017f; // Minimal exponential reduction

    // Add baseline and clamp for stability
    float distanceAttenuation = a / (b + c * dist + d * dist * dist + 1e-6f);

    // Smooth transition using smoothstep


    // Use smoothAttenuation for realistic falloff
    float transmittance = exp(-totalDensity * lightAbsorption * 1.2f) * distanceAttenuation ;
    return transmittance;
}

const glm::mat3 m3 = glm::mat3(
    glm::vec3(0.00f,  0.80f,  0.60f),
    glm::vec3(-0.80f, 0.36f, -0.48f),
    glm::vec3(-0.60f, -0.48f, 0.64f)
);

const glm::mat3 m3i = glm::mat3(
    glm::vec3(0.00f, -0.80f, -0.60f),
    glm::vec3(0.80f,  0.36f, -0.48f),
    glm::vec3(0.60f, -0.48f,  0.64f)
);
const glm::mat2 m2 = glm::mat2(  0.80,  0.60,
                      -0.60,  0.80 );

glm::vec3 fog(const glm::vec3& col, float t) {
    glm::vec3 ext = glm::exp(-t * 0.00025f * glm::vec3(1.0f, 1.5f, 4.0f));
    return col * ext + (1.0f - ext) * glm::vec3(0.55f, 0.55f, 0.58f);
}

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


glm::vec4 noised(const glm::vec3& x) {
    glm::vec3 p = glm::floor(x);
    glm::vec3 w = glm::fract(x);

    // Smooth interpolation
    glm::vec3 u = w * w * w * (w * (w * 6.0f - 15.0f) + 10.0f);
    glm::vec3 du = 30.0f * w * w * (w * (w - 2.0f) + 1.0f);

    // Hash-based gradient noise contributions
    float n = p.x + 317.0f * p.y + 157.0f * p.z;

    float a = hash1(n + 0.0f);
    float b = hash1(n + 1.0f);
    float c = hash1(n + 317.0f);
    float d = hash1(n + 318.0f);
    float e = hash1(n + 157.0f);
    float f = hash1(n + 158.0f);
    float g = hash1(n + 474.0f);
    float h = hash1(n + 475.0f);

    // Linear interpolation terms
    float k0 = a;
    float k1 = b - a;
    float k2 = c - a;
    float k3 = e - a;
    float k4 = a - b - c + d;
    float k5 = a - c - e + g;
    float k6 = a - b - e + f;
    float k7 = -a + b + c - d + e - f - g + h;

    // Final noise value
    float value = -1.0f + 2.0f * (k0 + k1 * u.x + k2 * u.y + k3 * u.z +
                                  k4 * u.x * u.y + k5 * u.y * u.z +
                                  k6 * u.z * u.x + k7 * u.x * u.y * u.z);

    // Gradient
    glm::vec3 gradient = 2.0f * du * glm::vec3(
        k1 + k4 * u.y + k6 * u.z + k7 * u.y * u.z,
        k2 + k5 * u.z + k4 * u.x + k7 * u.z * u.x,
        k3 + k6 * u.x + k5 * u.y + k7 * u.x * u.y
    );

    return glm::vec4(value, gradient);
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
// //OpenGL smoothstep
// float smoothstep(float edge0, float edge1, float x) {
//     // Clamp x to the range [edge0, edge1]
//     x = glm::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
//     return x * x * (3.0f - 2.0f * x);
// }
// //OpenGL smoothstep
// glm::vec3 smoothstep(float edge0, float edge1, glm::vec3 x) {
//     return glm::vec3(
//         smoothstep(edge0, edge1, x.x),
//         smoothstep(edge0, edge1, x.y),
//         smoothstep(edge0, edge1, x.z)
//     );
// }

glm::vec3 Cloud::renderClouds(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& lightPos, const glm::vec3& lightColor, const glm::vec3& backgroundColor, float radius) const 
{    

    // glm::vec3 col = glm::vec3(0.42,0.62,1.1) - glm::vec3(0.4f * rayDir.y);
    glm::vec3 col = backgroundColor;
    // clouds
    // float t = (2500.0-rayOrigin.y)/rayDir.y;
    // if( t>0.0 )
    // {
    //     glm::vec3 pos = rayOrigin + t * rayDir;
    //     glm::vec2 uv = glm::vec2(pos.x, pos.z);
    //     float cl = fbm_9( uv * 0.00104f );
    //     float dl = smoothstep(-0.2,0.6,cl);
    //     col = mix( col, glm::vec3(1.0), 0.12*dl );
    // }
    
    // // sun glare    
    // float sun = glm::clamp( glm::dot(kSunDir,rayDir), 0.0f, 1.0f );
    // col +=  0.2f * glm::vec3(1.0f, 0.6f, 0.3f) * glm::pow(sun, 32.0f);


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

    float epsilon = 1e-6f;
    tMin = glm::max(tMin, tzMin - epsilon);
    tMax = glm::min(tMax, tzMax + epsilon);

    if (tMin < 0 && tMax < 0) {
       hit= false; // Intersection happens behind the ray origin
    }

    if(hit == false){
        return col;
    }



    //start ray marching.
    // Ray intersects the box; start ray tracing within the box
    tMin = glm::max(0.f,tMin);
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
        //float density = glm::clamp(sampleDensity(rayPos) * stepSize, 0.0f, 1.0f);

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
          //agian, can repace col w. background color as before
    glm::vec3 finalColor = col * transmittance + cloudColor;


    return finalColor;
}



glm::vec4 fbmd_8(const glm::vec3& x) {
    float f = 2.0f;   // Frequency multiplier
    float s = 0.65f;  // Gain for amplitude
    float a = 0.0f;   // Accumulated noise value
    float b = 0.5f;   // Initial amplitude
    glm::vec3 d(0.0f); // Accumulated derivative
    glm::mat3 m(1.0f); // Identity matrix for initial transformation

    glm::vec3 currentX = x; // Work on a copy of x
    for (int i = 0; i < 8; ++i) {
        glm::vec4 n = noised(currentX); 
        a += b * n.x; // Accumulate noise value

        if (i < 4) {
            d += b * m * glm::vec3(n.y, n.z, n.w); // Accumulate derivatives
        }

        b *= s;              // Decrease amplitude
        currentX = f * m3 * currentX; // Transform coordinates for next octave
        m = f * m3i * m;     // Update transformation matrix
    }

    return glm::vec4(a, d);
}


glm::vec4 cloudsFbm(const glm::vec3& pos, const glm::vec3& offset) {
    glm::vec3 adjustedPos = pos * 0.0015f + glm::vec3(2.0f, 1.1f, 1.0f) + offset;
    return fbmd_8(adjustedPos);
}

glm::vec4 Cloud::cloudsMap(const glm::vec3& pos, float& nnd, const glm::vec3& offset) {
    // Initial distance to a flat cloud layer at y = 900, thickness of 40
   float d = glm::length((pos - center) / glm::vec3(length, breadth, height)) - breadth/4.f;
//   std::cout << d<< std::endl;
    glm::vec3 gra(0.0f, glm::sign(pos.y - (center.y - breadth / 2.f)), 0.0f);

    // Add fractal noise with offset

    glm::vec4 n = cloudsFbm(pos, offset);
    
    d += 400.0f * n.x * (0.7f + 0.3f * gra.y);
    
    // If the point is outside the cloud
    if (d > 0.0f) {
        return glm::vec4(-d, 0.0f, 0.0f, 0.0f);
    }
    // std::cout << d<< std::endl;
    // Update the absolute distance to the surface
    nnd = -d;

    // Scale density for blending
    d = std::min(-d / 100.0f, 0.25f);

    return glm::vec4(d, gra);
}


glm::vec3 Cloud::renderClouds(const glm::vec3& ro, const glm::vec3& rd, float tmin, float tmax, float resT, const glm::vec2& px, const glm::vec3& backgroundColor){
    glm::vec4 sum(0.0f);
    
    // float tl = (600.0f - ro.y) / rd.y;
    // float th = (1200.0f - ro.y) / rd.y;
    // if (tl > 0.0f) tmin = std::max(tmin, tl);
    // else return sum;
    // if (th > 0.0f) tmax = std::min(tmax, th);
    glm::vec3 minBounds = center - glm::vec3(length, breadth, height) * 0.5f;
    glm::vec3 maxBounds = center + glm::vec3(length, breadth, height) * 0.5f;

    // Perform ray-box intersection
    float tMin = (minBounds.x - ro.x) / rd.x;
    float tMax = (maxBounds.x - ro.x) / rd.x;
    if (tMin > tMax) std::swap(tMin, tMax);

    float tyMin = (minBounds.y - ro.y) / rd.y;
    float tyMax = (maxBounds.y - ro.y) / rd.y;
    if (tyMin > tyMax) std::swap(tyMin, tyMax);

    if ((tMin > tyMax) || (tyMin > tMax)) return backgroundColor; // No intersection
    tMin = std::max(tMin, tyMin);
    tMax = std::min(tMax, tyMax);

    float tzMin = (minBounds.z - ro.z) / rd.z;
    float tzMax = (maxBounds.z - ro.z) / rd.z;
    if (tzMin > tzMax) std::swap(tzMin, tzMax);

    if ((tMin > tzMax) || (tzMin > tMax)) return backgroundColor; // No intersection
    tMin = std::max(tMin, tzMin);
    tMax = std::min(tMax, tzMax);

    if (tMin < 0.0f && tMax < 0.0f) return backgroundColor; // Intersection behind the ray

    // Update the bounds for ray marching
    tmin = std::max(tmin, tMin);
    tmax = std::min(tmax, tMax);

    // Start ray marching logic (same as before)

    // std::cout<<"hello";
    float t = tmin;
    float lastT = -1.0f;
    float thickness = 0.0f;

    for (int i = 0; i < numSteps; i++) {
        glm::vec3 pos = ro + t * rd;

        // Get density and gradient from cloud map
        float nnd;
        glm::vec4 denGra = cloudsMap(pos, nnd, shapeOffset);
        float den = denGra.x;
        // std::cout << "Low or zero density at position: " << den << std::endl;

        float dt = std::max(0.2f, 0.011f * t);
        

        if (den > 0.001f) {
            
            // std::cout << "Low or zero density at position: " << den << std::endl;

            // Shadow computation
            float kk;
            cloudsMap(pos + glm::vec3(0.0f, 0.0f, -1.0f) * 70.0f, kk,  shapeOffset);
            float sha = 1.0f - glm::smoothstep(-200.0f, 200.0f, kk);
            sha *= 1.5f;

            // Compute normals and lighting
            glm::vec3 nor = glm::normalize(glm::vec3(denGra.y, denGra.z, denGra.w));
            float dif = glm::clamp(0.4f + 0.6f * glm::dot(nor, glm::vec3(0.0f, 0.0f, -1.0f)), 0.0f, 1.0f) * sha;
            float fre = glm::clamp(1.0f + glm::dot(nor, rd), 0.0f, 1.0f) * sha;
            float occ = 0.2f + 0.7f * std::max(1.0f - kk / 200.0f, 0.0f) + 0.1f * (1.0f - den);

            // Lighting
            glm::vec3 lin(0.0f);
            lin += glm::vec3(0.70f, 0.80f, 1.00f) * 1.0f * (0.5f + 0.5f * nor.y) * occ;
            lin += glm::vec3(0.10f, 0.40f, 0.20f) * 1.0f * (0.5f - 0.5f * nor.y) * occ;
            lin += glm::vec3(1.00f, 0.95f, 0.85f) * 3.0f * dif * occ + 0.1f;

            // Cloud color
            glm::vec3 col = glm::vec3(0.8f, 0.8f, 0.8f) * 0.45f;
            col *= lin;
            col = fog(col, t);

            // Front-to-back blending
            float alp = glm::clamp(den * 0.5f * 0.125f * dt, 0.0f, 1.0f);
            col *= alp;
            sum += glm::vec4(col, alp) * (1.0f - sum.a);

            thickness += dt * den;
            if (lastT < 0.0f) lastT = t;
        } else {
            dt = std::abs(den) + 0.2f;
        }

        t += dt;

        if (sum.a > 0.995f || t > tmax) break;
    }

    if (lastT > 0.0f) resT = std::min(resT, lastT);
            glm::vec3 sumVec3 = glm::vec3(sum.x, sum.y, sum.z);
            sumVec3 += std::max(0.0f, 1.0f - 0.0125f * thickness) * glm::vec3(1.00f, 0.60f, 0.40f) * 0.3f *
           glm::pow(glm::clamp(glm::dot(glm::vec3(0.0f, 0.0f, -1.0f), rd), 0.0f, 1.0f), 32.0f);   

           sumVec3 = glm::clamp(sumVec3, 0.f, 1.f);

    glm::vec3 finalcol = backgroundColor*(1.0f-sum.w) + sumVec3;
    return finalcol;
}



glm::vec3 Cloud::renderClouds(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& backgroundColor, std::vector<Light> l) const 
{    

    // glm::vec3 col = glm::vec3(0.42,0.62,1.1) - glm::vec3(0.4f * rayDir.y);
    glm::vec3 col = backgroundColor;
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

    float epsilon = 1e-6f;
    tMin = glm::max(tMin, tzMin - epsilon);
    tMax = glm::min(tMax, tzMax + epsilon);

    if (tMin < 0 && tMax < 0) {
       hit= false; // Intersection happens behind the ray origin
    }

    if(hit == false){
        return col;
    }



    //start ray marching.
    // Ray intersects the box; start ray tracing within the box
    tMin = glm::max(0.f,tMin);
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
        //float density = glm::clamp(sampleDensity(rayPos) * stepSize, 0.0f, 1.0f);

        if (density > 0.0f) {
            

            for (const auto& light : l) {
                // Get light properties
                glm::vec3 lightPos = light.pos;
                glm::vec3 lightColor = light.emissionColor;
                float radius = light.radius;

                // Calculate light transmittance for this light
                float lightTransmittance = lightMarch(rayPos, lightPos, radius);

                // Accumulate energy contribution
                lightEnergy += density * stepSize * transmittance * lightTransmittance * lightColor;
            }


            // Update transmittance
            transmittance *= exp(-density * stepSize * lightAbsorption);

            // Early exit if transmittance is negligible
            if (transmittance < 0.01f)
                break;
        }

        dstTravelled += stepSize;
    }



    glm::vec3 cloudColor = lightEnergy;
          //agian, can repace col w. background color as before
    glm::vec3 finalColor = col * transmittance + cloudColor;


    return finalColor;
}
