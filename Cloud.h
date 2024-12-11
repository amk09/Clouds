#ifndef CLOUD_H
#define CLOUD_H

#include "src/rgba.h"
#include "camera.h"
#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"
#include "helper.h"
#include "lights.h"
#include "Weathermap.h"
#include "Noisemap.h"
#include "DetailNoisemap.h"
#include <vector>
#include <random>



class Cloud {
public:
    Cloud(glm::vec3 center, float length, float breadth, float height, float densityOffset, glm::vec3 shapeOffset, float densityMultiplier, float lightAbsorption);
    

    float sampleDensity(glm::vec3 position) const;
    float lightMarch(glm::vec3 position, glm::vec3 dirToLight, float radius) const;
    glm::vec3 renderClouds(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& lightDir, const glm::vec3& lightColor, const glm::vec3& backgroundColor,  float radius) const;
    glm::vec3 renderClouds(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& backgroundColor, std::vector<Light> l) const;

    glm::vec3 renderClouds(const glm::vec3& ro, const glm::vec3& rd, float tmin, float tmax, float resT, const glm::vec2& px, const glm::vec3& backgroundColor);
    glm::vec4 cloudsMap(const glm::vec3& pos, float& nnd, const glm::vec3& offset);

    glm::vec3 center;
    float length, breadth, height;
    float densityOffset;
    glm::vec3 shapeOffset;
    float densityMultiplier;
    float lightAbsorption;
    int numSteps;
    int numStepsLight;
    const glm::vec3 kSunDir = glm::normalize(glm::vec3(-0.7071,0.0,-0.7071));

    /*
    Cloud global variable definition
    */
    float gc = 1.f; //global coverage controls the basic probability for clouds to appear. \in [0,1]
    float gd = 3.f; // global density term gd in (0, inf) determines the clouds global opacity

    std::mt19937 rng = std::mt19937(42);                           // Random number generator
    std::uniform_real_distribution<float> dist = std::uniform_real_distribution<float>(0.0f, 1.0f);  // Uniform distribution
    std::mt19937 gen = std::mt19937(42);
    float cloudAnvilAmount;  // Cloud anvil amount

    WeatherMap weatherMap;   // Weather map object
    void generateWeatherMap() {
        for (int y = 0; y < weatherMap.getHeight(); ++y) {
            for (int x = 0; x < weatherMap.getWidth(); ++x) {
                 float redValue = dist(rng);    // Low coverage
            float greenValue = dist(rng);  // High coverage
            float blueValue = dist(rng);   // Maximum cloud height
            float alphaValue = dist(rng);  // Cloud density

            // // Randomize red and green channels for cloud appearance probability
            weatherMap.setPixel(x, y, 'r', redValue);   // Red: low coverage
            weatherMap.setPixel(x, y, 'g', greenValue); // Green: high coverage
            // // Generate height and density values
            weatherMap.setPixel(x, y, 'b', blueValue);  // Blue: maximum cloud height
            weatherMap.setPixel(x, y, 'a', alphaValue); // Alpha: cloud density
                
    
            }
        }
    }

    /*
    NoiseMap variable
    */
    NoiseMap noiseMap;
    void GenerateNoiseMap() {
    
        int length = noiseMap.getWidth();
        int height = noiseMap.getHeight();
        int depth = noiseMap.getDepth();

        // Loop through each voxel and assign values to the NoiseMap channels
        for (int x = 0; x < length; ++x) {
            for (int y = 0; y < height; ++y) {
                for (int z = 0; z < depth; ++z) {
                    // float offsetX = dist(gen) * 10.0f;
                    // float offsetY = dist(gen) * 10.0f;
                    // float offsetZ = dist(gen) * 10.0f;
                    // glm::vec3 position = glm::vec3(x, y, z) + glm::vec3(offsetX, offsetY, offsetZ);
                    // Low frequency Perlin-Worley noise
                    float lowFreq = glm::perlin(glm::vec3(x, y, z) * 0.01f) * 0.5f + 0.5f;
                    noiseMap.setPixel(x, y, z, 'r', lowFreq);
                    // Medium frequency Worley noise
                    float mediumFreq = glm::perlin(glm::vec3(x, y, z) * 0.05f) * 0.5f + 0.5f;
                    noiseMap.setPixel(x, y, z, 'g', mediumFreq);
                    // High frequency Worley noise
                    float highFreq = glm::perlin(glm::vec3(x, y, z) * 0.1f) * 0.5f + 0.5f;
                    noiseMap.setPixel(x, y, z, 'b', highFreq);
                    // Highest frequency Worley noise
                    float alphaFreq = glm::perlin(glm::vec3(x, y, z) * 0.2f) * 0.5f + 0.5f;
                    noiseMap.setPixel(x, y, z, 'a', alphaFreq);
                }
            }
        }
    }

    DetailNoiseMap detailNoisemap;
    void GenerateDetailNoiseMap() {
        int length = detailNoisemap.getWidth();
        int height = detailNoisemap.getHeight();
        int depth = detailNoisemap.getDepth();

        // Loop through each voxel and assign values to the DetailNoiseMap channels
        for (int x = 0; x < length; ++x) {
            for (int y = 0; y < height; ++y) {
                for (int z = 0; z < depth; ++z) {
                    // Low frequency Worley noise
                    float lowFreq = glm::perlin(glm::vec3(x, y, z) * 0.01f) * 0.5f + 0.5f;
                    detailNoisemap.setPixel(x, y, z, 'r', lowFreq);
                    // Medium frequency Worley noise
                    float mediumFreq = glm::perlin(glm::vec3(x, y, z) * 0.05f) * 0.5f + 0.5f;
                    detailNoisemap.setPixel(x, y, z, 'g', mediumFreq);

                    // High frequency Worley noise
                    float highFreq = glm::perlin(glm::vec3(x, y, z) * 0.1f) * 0.5f + 0.5f;
                    detailNoisemap.setPixel(x, y, z, 'b', highFreq);
                }
            }
        }
    }



    //helper funcitons for noise map



    /*
    Helper function
    */
    float saturate(float value) const{
    return glm::clamp(value, 0.0f, 1.0f);
    }
    // Linear interpolation function (lerp equivalent in HLSL)
    float lerp(float v0, float v1, float i) const{
        return (1-i)*v0 + i*v1;
    }

    // ReMap function to remap a value from one range to another
    float ReMap(float v, float l0, float ho, float ln, float hn) const{
        return ln+(((v-l0)*(hn-ln))/(ho-l0));
    }


    /*
    WeatherMap
    */

    //function 4, Wc_c value
    float WM_c (WeatherMap wm, int x, int y){
        float wc0 = wm.getPixel(x, y, 'r'); // Red channel
        float wc1 = wm.getPixel(x, y, 'g'); // Green channel

        // Compute SAT(gc - 0.5) * wc1 * 2
        float satValue = saturate(gc-0.5f);
        float secondaryCoverage = satValue * wc1 * 2.0f;
        return std::max(wc0, secondaryCoverage);
    }


    /*
    Height-dependent functions
    */

    //function7: shape-altering height-function
    //av = 0 gc = 1 for cloud that lacks resemblance with cumulonimbus clouds; av in [0,1]
    float HeightAlter(float percentHeight, WeatherMap wm, float av, int x, int y) {
        // Round bottom a bit
        float ret_val = saturate(ReMap(percentHeight, 0.0f, 0.07f, 0.0f, 1.0f));

        // Round top a lot
        float stopHeight = saturate(wm.getPixel(x,y, 'b') + 0.12f);

        ret_val *= saturate(ReMap(percentHeight, stopHeight * 0.2f, stopHeight, 1.0f, 0.0f));

        // Apply anvil (cumulonimbus/"giant storm" clouds)
        ret_val = pow(ret_val, saturate(ReMap(percentHeight, 0.65f, 0.95f, 1.0f, 1.0f - av * gc)));

        return ret_val;
    }

    //Density-altering height-function: make more fluffy at the bottom and have more defined shapes towards the top a height-dependent density function is used.
    float DensityAlter(float percentHeight, WeatherMap wm, float cloudAnvilAmount, int x, int y) const{

        //   Height dependent function that alters the cloud density in regards to the 0 to 1
            // height percentage (the percentage of on what altitude in the 400-1000 meters
            // the sample point is).

        // Have density generally increasing over height
        float ret_val = percentHeight;

        // Reduce density at base
        ret_val *= saturate(ReMap(percentHeight, 0.0f, 0.2f, 0.0f, 1.0f));

        // Apply weather map density
        ret_val *= wm.getPixel(x, y, 'a') * 2.0f;

        // Reduce density for the anvil (cumulonimbus clouds); which handles the case of a -> 1. With av = 1, the function reduces the density with a multiplier going towards 0.2 
        ret_val *= lerp(1.0f, saturate(ReMap(pow(percentHeight, 0.5f), 0.4f, 0.95f, 1.0f, 0.2f)), cloudAnvilAmount);

        // Reduce density at top to make a better transition
        ret_val *= saturate(ReMap(percentHeight, 0.9f, 1.0f, 1.0f, 0.0f));

        return ret_val;
    }


    float shape_noise(const NoiseMap& nm, int x, int y, int z) const{
    float sn = nm.getPixel(x, y, z, 'g') * 0.625f + 
               nm.getPixel(x, y, z, 'b') * 0.25f + 
               nm.getPixel(x, y, z, 'a') * 0.125f;
    float ssn = -(1 - sn);
    return ReMap(nm.getPixel(x, y, z, 'r'), ssn, 1.0f, 0.0f, 1.0f);
}


    
    //function 13
    float d(const DetailNoiseMap& dn, const NoiseMap& nm, const WeatherMap& wm, 
               int world_x, int world_y, int world_z, float percentHeight, float cloudAnvilAmount) const {
    float world_min_x = center.x - length / 2.0f;
    float world_min_y = center.y - breadth / 2.0f;
    float world_min_z = center.z - height / 2.0f;

    float world_max_x = center.x + length / 2.0f;
    float world_max_y = center.y + breadth / 2.0f;
    float world_max_z = center.z + height / 2.0f;

    // Map world coordinates to the respective grid coordinates
    int x_dn = static_cast<int>(((world_x - world_min_x) / (world_max_x - world_min_x)) * dn.getWidth());
    int y_dn = static_cast<int>(((world_y - world_min_y) / (world_max_y - world_min_y)) * dn.getHeight());
    int z_dn = static_cast<int>(((world_z - world_min_z) / (world_max_z - world_min_z)) * dn.getDepth());

    int x_nm = static_cast<int>(((world_x - world_min_x) / (world_max_x - world_min_x)) * nm.getWidth());
    int y_nm = static_cast<int>(((world_y - world_min_y) / (world_max_y - world_min_y)) * nm.getHeight());
    int z_nm = static_cast<int>(((world_z - world_min_z) / (world_max_z - world_min_z)) * nm.getDepth());

    int x_wm = static_cast<int>(((world_x - world_min_x) / (world_max_x - world_min_x)) * wm.getWidth());
    int y_wm = static_cast<int>(((world_y - world_min_y) / (world_max_y - world_min_y)) * wm.getHeight());

    // Ensure the mapped coordinates are within bounds
    x_dn = glm::clamp(x_dn, 0, dn.getWidth() - 1);
    y_dn = glm::clamp(y_dn, 0, dn.getHeight() - 1);
    z_dn = glm::clamp(z_dn, 0, dn.getDepth() - 1);

    x_nm = glm::clamp(x_nm, 0, nm.getWidth() - 1);
    y_nm = glm::clamp(y_nm, 0, nm.getHeight() - 1);
    z_nm = glm::clamp(z_nm, 0, nm.getDepth() - 1);

    x_wm = glm::clamp(x_wm, 0, wm.getWidth() - 1);
    y_wm = glm::clamp(y_wm, 0, wm.getHeight() - 1);

    // Perform the calculations using the mapped coordinates
    float detail_noise = dn.getPixel(x_dn, y_dn, z_dn, 'r') * 0.625f + 
                         dn.getPixel(x_dn, y_dn, z_dn, 'g') * 0.25f + 
                         dn.getPixel(x_dn, y_dn, z_dn, 'b') * 0.125f;

    float detail_modifier = lerp(detail_noise, 1 - detail_noise, saturate(percentHeight * 5.0f));
    detail_modifier *= 0.35f * exp(gc * -0.75f);

    float final_density = saturate(ReMap(shape_noise(nm, x_nm, y_nm, z_nm), detail_modifier, 1.0f, 0.0f, 1.0f));

    return final_density * DensityAlter(percentHeight, wm, cloudAnvilAmount, x_wm, y_wm);
}

};

#endif // CLOUD_H


