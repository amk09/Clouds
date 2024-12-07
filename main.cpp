// #include "glm/gtx/transform.hpp"
// #include <iostream>
// #include <vector>
// #include "camera.h"
// #include "src/rgba.h"
// #include "src/backend.h"
// #include "Cloud.h"
// #include "lights.h"


// int main()
// {
//     int height = 1080, width = 1920;
//     std::vector<RGBA> Image(width * height, RGBA(0, 0, 0, 255));

//     glm::vec3 cloudCenter(0.0f, 0.0f, -10.f); 

//     Camera camera;
//     // camera.pos = glm::vec4(30.0f, 20.0f, 30.0f, 1.0f); // Position the camera
//     camera.pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
//     // camera.look = glm::normalize(glm::vec4(cloudCenter, 1.0f) - camera.pos); // Look at the cloud box
//     camera.look = glm::normalize(glm::vec4(0.0f,0.f,-1.f,0.f));

//     camera.up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // Keep the camera upright

//     // float k = glm::length(glm::vec3(camera.pos - glm::vec4(0.0f)));
//     float k = 0.1f;
//     float horizontal_angle = 45.0;
//     float V = 2 * k * glm::tan(glm::radians(horizontal_angle / 2));
//     float U = (width * V) / height;

//     glm::vec4 worldEye = camera.getViewMatrixInverse() * camera.pos;
//     Light light1(glm::vec3(0.f,0.f,-10.f), glm::vec3(1.0f,0.0f,0.0f), 0.2f);

//     float length = 20.0f, breadth = 20.0f, h = 20.0f;
//     float densityOffset = 0.1f, densityMultiplier = 0.8f, lightAbsorption  = 0.1f;

                            
//     glm::vec3 backgroundColor(0.0f, 0.00f, 0.0f); 

//     Cloud cloud(cloudCenter, length, breadth, h, densityOffset, densityMultiplier, lightAbsorption); 

//     // OpenMP parallel loop
//     #pragma omp parallel for collapse(2) schedule(dynamic)
    
//     for (int j = 0; j < height; j++)
//     {
//         for (int i = 0; i < width; i++)
//         {
//             float x = ((i + 0.5) / width) - 0.5;
//             float y = ((height - 1 - j + 0.5) / height) - 0.5;

//             glm::vec4 uvk(U * x, V * y, -k, 1.f);

//             glm::vec4 raydir = glm::normalize((uvk - camera.pos));
//             glm::vec4 worldRayDir = glm::normalize(camera.getViewMatrixInverse() * raydir); // to world space

//             Image[j * width + i] =  convertVec3RGBA( cloud.renderClouds(glm::vec3(worldEye), glm::vec3(worldRayDir), light1.pos, light1.emissionColor, backgroundColor,light1.radius) + light1.lightSphereWithGlow(worldRayDir,worldEye));
//         }
//     }

//     // Save the image after processing
//     saveImage(Image,"output.png");

//     return 0;
// }


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
#include "lights.h"

int main()
{
    int height = 720, width = 1280;
    std::vector<RGBA> Image(width * height, RGBA(0, 0, 0, 255));

    glm::vec3 cloudCenter(0.0f, 0.0f, -10.f); 

    Camera camera;
    camera.pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    camera.look = glm::normalize(glm::vec4(0.0f,0.f,-1.f,0.f));
    camera.up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); 

    float k = 0.1f;
    float horizontal_angle = 45.0;
    float V = 2 * k * glm::tan(glm::radians(horizontal_angle / 2));
    float U = (width * V) / height;

    glm::vec4 worldEye = camera.getViewMatrixInverse() * camera.pos;

    float length = 20.0f, breadth = 20.0f, h = 20.0f;
    float densityOffset = 0.1f, densityMultiplier = 0.8f, lightAbsorption  = 0.1f;
    glm::vec3 backgroundColor(0.0f, 0.00f, 0.0f); 

    Cloud cloud(cloudCenter, length, breadth, h, densityOffset, densityMultiplier, lightAbsorption);

    // Define start and end positions for the light
    glm::vec3 startPos = glm::vec3(0.f,-3.f,-10.f);   // at camera
    glm::vec3 endPos   = glm::vec3(0.f,5.f,-10.f); // well beyond the cloud center
    glm::vec3 dir = glm::normalize(endPos - startPos);
    int totalFrames = 1;
    glm::vec3 red(1.0f, 0.0f, 0.0f);
    glm::vec3 orange(1.0f, 0.5f, 0.0f);
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
            emissionColor = glm::mix(orange, white, localT);
        }

        float lightRadius = 0.2f;
        Light light1(currentLightPos, emissionColor, lightRadius);

        // Render the frame
        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                float x = ((i + 0.5f) / width) - 0.5f;
                float y = ((height - 1 - j + 0.5f) / height) - 0.5f;

                glm::vec4 uvk(U * x, V * y, -k, 1.f);
                glm::vec4 raydir = glm::normalize((uvk - camera.pos));
                glm::vec4 worldRayDir = glm::normalize(camera.getViewMatrixInverse() * raydir);

                glm::vec3 color = cloud.renderClouds(glm::vec3(worldEye), glm::vec3(worldRayDir), 
                                                     light1.pos, light1.emissionColor, backgroundColor, light1.radius)
                                   + light1.lightSphereWithGlow(worldRayDir, worldEye);

                Image[j * width + i] = convertVec3RGBA(color);
            }
        }

        // Save the image with a frame-based filename, e.g., frame_000.png
        char filename[64];
        snprintf(filename, sizeof(filename), "frame_%03d.png", frame);
        saveImage(Image, filename);
        std::cout<<"Frame "<<frame<<" generated"<<std::endl;
    }

    return 0;
}