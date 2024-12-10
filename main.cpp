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



int main()
{
    // Output setup     
    int height = 480;
    int width = 640;

    glm::vec3 backgroundColor(0.f,0.f,0.f); //(0.5f, 0.7f, 1.0f); 
    std::vector<RGBA> Image(width * height, RGBA(0, 0, 0, 255));

    // Light setup

    glm::vec3 lightPos(0.f,0.0f,-10.f);
    glm::vec3 ligthColor(1.f,0.f,1.f);
    float radius(0.2f);

    //Light light1(lightPos, ligthColor, radius);

    // Camera setup

    Camera camera;
    camera.pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); 
    camera.look = glm::normalize(glm::vec4(0.f,0.f,-1.f,0.f)); 
    camera.up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); 

    float k = 0.1f;
    float horizontal_angle = 30.0;
    float V = 2 * k * glm::tan(glm::radians(horizontal_angle / 2));
    float U = (width * V) / height;

    glm::vec4 worldEye = camera.pos;


    // Clouds setup 

    
    float length = 1.f;                    // Lenght  is along the x axis
    float breadth = 1.f;                   // Breadth is along the y axis 
    float h = 1.f;                        // Height  is along the z axis
    float densityOffset = 0.1f;             // Changes the density of cloud, less is more dense
    float densityMultiplier = 1.f;          // Increasing would increase density
    float lightAbsorption = 0.7f;           // Increasing would darken the clouds  ; ideas : can tweak this value to make rain
    glm::vec3 shapeOffset(0.f, 0.f, 0.f);   // Movement x,y,z for directional movements


    
                

    // Define start and end positions for the light
    // glm::vec3 startPos = glm::vec3(0.f,0.-0.7f,0.f);   // at camera
    // glm::vec3 endPos   = glm::vec3(0.f,.3f,0.f); // well beyond the cloud center
    // glm::vec3 dir = glm::normalize(endPos - startPos);
    // int totalFrames = 24;
    // glm::vec3 red(1.0f, 1.0f, 1.0f);
    // glm::vec3 orange(1.0f, 1.f, 1.0f);
    // glm::vec3 white(1.0f, 1.0f, 1.0f);

    omp_set_num_threads(omp_get_max_threads());
    float time = 0.0f; // Initialize time
    int frames_to_render = 240;
    float x = 10.f;

    for (int frame = 0; frame < frames_to_render; ++frame) 
    { 
        glm::vec3 cloudCenter(0.f, 0.f, -10.0f); 
        Cloud cloud(cloudCenter, length, breadth, h, densityOffset, shapeOffset , densityMultiplier, lightAbsorption);
        cloud.shapeOffset.x += 0.1f; // Updating cloud positions
        
        lightPos = glm::vec3(0.0f,0.0f,-20.f - x);
        x -= 0.5f;
        Light light1(lightPos, ligthColor, radius);


        // OpenMP parallel loop for rendering
        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (int j = 0; j < height; j++) 
        {
            for (int i = 0; i < width; i++) 
            {

                float x = ((i + 0.5f) / width) - 0.5f;
                float y = ((height - 1 - j + 0.5f) / height) - 0.5f;

                // Shooting rays from center to viewport
                glm::vec4 uvk(U * x, V * y, -k, 1.f);
                glm::vec4 raydir = glm::normalize((uvk - camera.pos));
                glm::vec4 worldRayDir = glm::normalize(camera.getViewMatrixInverse() * raydir); 

                // Render pixel using the updated cloud position
                // Image[j * width + i] = raymarchCloud(
                //     glm::vec3(worldEye), glm::vec3(worldRayDir), cloud, light1.pos, light1.emissionColor, backgroundColor, light1.radius);


                glm::vec3 cloudDisplay = cloud.renderClouds(glm::vec3(worldEye), glm::vec3(worldRayDir), light1.pos, light1.emissionColor, backgroundColor, light1.radius);
                glm::vec3 lightDisplay = light1.lightSphereWithGlow(worldRayDir, camera.pos);
                glm::vec3 surfColor = implicitPlaneIntersect(glm::vec3(worldEye), glm::vec3(worldRayDir),-5.f, light1.pos, light1.emissionColor, glm::vec3(1.f,1.f,1.f));
                //glm::vec3 surfColor = implicitWavySurfaceIntersect(glm::vec3(worldEye), glm::vec3(worldRayDir), -5.f, light1.pos, light1.emissionColor, glm::vec3(1.f,1.f,1.f));
                Image[j * width + i] = convertVec3RGBA(cloudDisplay + lightDisplay + surfColor);
            }
        }

        // Save the frame

        std::string filename = "output/cloud_frame_" + std::to_string(frame) + ".png";
        saveImage(Image, filename.c_str(), width, height);
        std::cout << "Saved frame: " << filename << std::endl;
    }


    return 0;
}
