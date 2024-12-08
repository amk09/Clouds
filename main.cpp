#include <omp.h>
#include <cstdio>
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <iostream>

#include "camera.h"
#include "src/rgba.h"
#include "src/backend.h"
#include "Cloud.h"

RGBA raymarchCloud(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const Cloud& cloud, const glm::vec3& lightDir, const glm::vec3& lightColor, const glm::vec3& backgroundColor) {
    glm::vec3 color = cloud.renderClouds(rayOrigin, rayDir, lightDir, lightColor, backgroundColor);

    
    int r = std::min(255, static_cast<int>(color.r * 255));
    int g = std::min(255, static_cast<int>(color.g * 255));
    int b = std::min(255, static_cast<int>(color.b * 255));
    return RGBA(r, g, b, 255);
}


int main()
{
    Camera camera;

    // Camera setup: Play with the camera to see 
    camera.pos = glm::vec4(30.0f, 20.0f, 30.0f, 1.0f); // Position the camera
    glm::vec3 cloudCenter(0.0f, -10.0f, 0.0f); 
    camera.look = glm::normalize(glm::vec4(cloudCenter, 1.0f) - camera.pos); // Look at the cloud box
    camera.up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // Keep the camera upright

    int height = 1000, width = 1000;
    std::vector<RGBA> Image(width * height, RGBA(0, 0, 0, 255));

    glm::vec3 cloudCenter(0.0f, 0.1f, 0.f); 
    float length = .1f, breadth = .1f, h = 10.f; // h is along z axis, b is along y axis, l is along x


    Camera camera;
    camera.pos = glm::vec4(0.0f, 0.0f, 10.f, 1.0f);
    // camera.look = glm::normalize(glm::vec4(cloudCenter,1.f)-camera.pos);
    camera.look = glm::normalize(glm::vec4(0.f,0.f,-1.f,0.0f));

    glm::vec3 worldUp(0.0f, 1.0f, 0.0f); 
    glm::vec3 right_vector = glm::normalize(glm::cross(glm::vec3(camera.look), worldUp));
    camera.up = glm::vec4(glm::normalize(glm::cross(right_vector, glm::vec3(camera.look))),0.f);

    float k = 0.1f;
    float horizontal_angle = 60.0;
    float V = 2 * k * glm::tan(glm::radians(horizontal_angle / 2));
    float U = (width * V) / height;

    glm::vec4 worldEye = camera.getViewMatrixInverse() * camera.pos;


    float densityOffset = 0.1f, densityMultiplier = 0.8f, lightAbsorption  = 0.2f;
    glm::vec3 backgroundColor(0.0f, 0.0f, 0.0f); 

    Cloud cloud(cloudCenter, length, breadth, h, densityOffset, densityMultiplier, lightAbsorption);

    // Define start and end positions for the light
    glm::vec3 startPos = glm::vec3(0.f,0.-0.7f,0.f);   // at camera
    glm::vec3 endPos   = glm::vec3(0.f,.3f,0.f); // well beyond the cloud center
    glm::vec3 dir = glm::normalize(endPos - startPos);
    int totalFrames = 24;
    glm::vec3 red(1.0f, 1.0f, 1.0f);
    glm::vec3 orange(1.0f, 1.f, 1.0f);
    glm::vec3 white(1.0f, 1.0f, 1.0f);

    for (int frame = 0; frame < totalFrames; ++frame) {
        float t = float(frame) / float(totalFrames - 1);
        glm::vec3 currentLightPos = startPos + dir * t;

        glm::vec3 emissionColor;
        if (t < 0.5f) {
            // Interpolate between red and orange
            float localT = t / 0.5f; // Maps [0, 0.5] to [0, 1]
            emissionColor = glm::mix(red, orange, localT);
        } else {
            // Interpolate between orange and white
            float localT = (t - 0.5f) / 0.5f; // Maps [0.5, 1.0] to [0, 1]
            emissionColor = glm::mix(red, orange, localT);
        }

        float lightRadius = 0.01f;
        Light light1(currentLightPos, emissionColor, lightRadius);
        //Light light1(glm::vec4(0.f,0.f,0.f,1.f), emissionColor, 0.01f);
        // Render the frame
        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (int j = 0; j < height; j++)
        {   
            for (int i = 0; i < width; i++)
            {
                float x = ((i + 0.5f) / width) - 0.5f;
                float y = ((height - 1 - j + 0.5f) / height) - 0.5f;


                glm::vec4 uvk(U * x, V * y, -k, 1.f);

                glm::vec4 raydir = glm::normalize((uvk - eye));
                glm::vec4 worldRayDir = glm::normalize(camera.getViewMatrixInverse() * raydir); // To world space

                // Render pixel using the updated cloud position
                Image[j * width + i] = raymarchCloud(
                    glm::vec3(worldEye), glm::vec3(worldRayDir), cloud, lightDir, lightColor, backgroundColor);
            }
        }

        // Save the image with a frame-based filename, e.g., frame_000.png
        char filename[64];
        snprintf(filename, sizeof(filename), "output/frame_%03d.png", frame);
        saveImage(Image, filename);
        std::cout<<"Frame "<<frame<<" generated"<<std::endl;
    }

    return 0;
}
