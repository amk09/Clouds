#include "glm/gtx/transform.hpp"
#include <iostream>
#include <vector>
#include "camera.h"
#include "src/rgba.h"
#include "src/backend.h"
#include "Cloud.h"
#include "lights.h"


int main()
{
    int height = 1000, width = 1000;
    std::vector<RGBA> Image(width * height, RGBA(0, 0, 0, 255));

    Camera camera;
    camera.pos = glm::vec4(30.0f, 20.0f, 30.0f, 1.0f); // Position the camera
    glm::vec3 cloudCenter(0.0f, -10.0f, 0.0f); 
    camera.look = glm::normalize(glm::vec4(cloudCenter, 1.0f) - camera.pos); // Look at the cloud box
    camera.up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // Keep the camera upright

    float k = glm::length(glm::vec3(camera.pos - glm::vec4(0.0f)));
    float horizontal_angle = 45.0;
    float V = 2 * k * glm::tan(glm::radians(horizontal_angle / 2));
    float U = (width * V) / height;

    glm::vec4 worldEye = camera.getViewMatrixInverse() * camera.pos;
    Light light1(glm::vec3(10.f,-10.f,-0.f), glm::vec3(1.0f,1.0f,0.0f), 0.2f);

    float length = 20.0f, breadth = 20.0f, h = 20.0f;
    float densityOffset = 0.1f, densityMultiplier = 0.8f, lightAbsorption  = 0.1f;

                            
    glm::vec3 backgroundColor(0.0f, 0.00f, 0.0f); 

    Cloud cloud(cloudCenter, length, breadth, h, densityOffset, densityMultiplier, lightAbsorption); 

    // OpenMP parallel loop
    #pragma omp parallel for collapse(2) schedule(dynamic)
    
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            float x = ((i + 0.5) / width) - 0.5;
            float y = ((height - 1 - j + 0.5) / height) - 0.5;

            glm::vec4 uvk(U * x, V * y, -k, 1.f);

            glm::vec4 raydir = glm::normalize((uvk - camera.pos));
            glm::vec4 worldRayDir = glm::normalize(camera.getViewMatrixInverse() * raydir); // to world space

            Image[j * width + i] =  convertVec3RGBA( cloud.renderClouds(glm::vec3(worldEye), glm::vec3(worldRayDir), light1.pos, light1.emissionColor, backgroundColor) + light1.lightSphereWithGlow(worldRayDir,worldEye));
        }
    }

    // Save the image after processing
    saveImage(Image);

    return 0;
}