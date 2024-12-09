#include <omp.h>
#include <cstdio>
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/string_cast.hpp"

#include <iostream>

#include "camera.h"
#include "src/rgba.h"
#include "src/backend.h"
#include "Cloud.h"

RGBA raymarchCloud(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const Cloud& cloud, const glm::vec3& lightDir, const glm::vec3& lightColor, const glm::vec3& backgroundColor, float radius) {
    glm::vec3 color = cloud.renderCloudsDirlight(rayOrigin, rayDir, lightDir, lightColor, backgroundColor);

    
    int r = std::min(255, static_cast<int>(color.r * 255));
    int g = std::min(255, static_cast<int>(color.g * 255));
    int b = std::min(255, static_cast<int>(color.b * 255));
    return RGBA(r, g, b, 255);
}


int main()
{
    Camera camera;
   camera.pos = glm::vec4(10.0f, -5.0f, 20.0f, 1.0f);  // Adjust for better visibility

    glm::vec3 cloudCenter(0.0f, -10.0f, 0.0f); 
    camera.look = glm::normalize(glm::vec4(cloudCenter - glm::vec3(camera.pos), 1.f));
    camera.up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // Keep the camera upright
    glm::vec3 expectedLook = glm::normalize(cloudCenter - glm::vec3(camera.pos));

    int height = 1000, width = 1000;
    std::vector<RGBA> Image(width * height, RGBA(0, 0, 0, 255));

    //float length = .1f, breadth = .1f, h = 10.f; // h is along z axis, b is along y axis, l is along x

    float k = 0.1f;
   float horizontal_angle = 90.0f;


    float V = 2 * k * glm::tan(glm::radians(horizontal_angle / 2));
    float U = (width * V) / height;

    glm::vec4 worldEye = camera.getViewMatrixInverse() * camera.pos;

    
    float length = 10.0f, breadth = 10.0f, h = 10.0f;

    float densityOffset = 0.3f, densityMultiplier = 0.8f, lightAbsorption = 0.5f;
    glm::vec3 shapeOffset = glm::vec3(0.f, 0.f, 0.f);
    Cloud cloud(cloudCenter, length, breadth, h, densityOffset, shapeOffset , densityMultiplier, lightAbsorption);
    glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)); // Light direction
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);                              // White light

    glm::vec3 backgroundColor(0.f,0.f,0.f);//(0.5f, 0.7f, 1.0f);             

    // Define start and end positions for the light
    // glm::vec3 startPos = glm::vec3(0.f,0.-0.7f,0.f);   // at camera
    // glm::vec3 endPos   = glm::vec3(0.f,.3f,0.f); // well beyond the cloud center
    // glm::vec3 dir = glm::normalize(endPos - startPos);
    // int totalFrames = 24;
    // glm::vec3 red(1.0f, 1.0f, 1.0f);
    // glm::vec3 orange(1.0f, 1.f, 1.0f);
    // glm::vec3 white(1.0f, 1.0f, 1.0f);


    float time = 0.0f; // Initialize time

    Light light1(glm::vec4(0.f,0.f,-0.10f,1.f), glm::vec3(1.0f, 0.0f, 0.0f), 0.1f);


    std::cout << "Camera Position: " << glm::to_string(glm::vec3(camera.pos)) << std::endl;
std::cout << "Cloud Center: " << glm::to_string(cloudCenter) << std::endl;
std::cout << "Look Direction: " << glm::to_string(glm::vec3(camera.look)) << std::endl;
    std::cout << "Horizontal FOV: " << horizontal_angle << ", U: " << U << ", V: " << V << std::endl;




    for (int frame = 0; frame < 20; ++frame) { // Render 100 frames
        // Update cloud position
        cloud.shapeOffset.x += 0.1; // Add dynamic offsets


        // OpenMP parallel loop for rendering
        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                float x = ((i + 0.5f) / width) - 0.5f;
                float y = ((height - 1 - j + 0.5f) / height) - 0.5f;

                glm::vec4 uvk(U * x, V * y, -k, 1.f);

                glm::vec4 raydir = glm::normalize((uvk - camera.pos));
                glm::vec4 worldRayDir = glm::normalize(camera.getViewMatrixInverse() * raydir); // To world space

                // Render pixel using the updated cloud position
                Image[j * width + i] = raymarchCloud(
                    glm::vec3(worldEye), glm::vec3(worldRayDir), cloud, lightDir, lightColor, backgroundColor, 1.0f);

            }
        }

        // Save the frame
        std::string filename = "output/cloud_frame_" + std::to_string(frame) + ".png";
        saveImage(Image, filename.c_str());
        std::cout << "Saved frame: " << filename << std::endl;
    }


    return 0;
}
