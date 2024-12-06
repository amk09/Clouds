#include "glm/gtx/transform.hpp"
#include <iostream>
#include <vector>
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

    int height = 1000, width = 1000;
    std::vector<RGBA> Image(width * height, RGBA(0, 0, 0, 255));

    float k = .1f;
    float horizontal_angle = 45.0;
    float V = 2 * k * glm::tan(glm::radians(horizontal_angle / 2));
    float U = (width * V) / height;

    glm::vec4 eye(0.f, 0.f, 0.f, 1.f);
    glm::vec4 worldEye = camera.getViewMatrixInverse() * eye;

    glm::vec3 cloudCenter(0.0f, 0.0f, -10.0f); 
    float length = 20.0f, breadth = 20.0f, h = 20.0f;
    float densityOffset = 0.2f, densityMultiplier = 1.0f, lightAbsorption = 0.1f;
    Cloud cloud(cloudCenter, length, breadth, h, densityOffset, densityMultiplier, lightAbsorption);

    glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)); // Light direction
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);                              // White light
    glm::vec3 backgroundColor(0.5f, 0.7f, 1.0f);                        // Sky blue background


    // OpenMP parallel loop
    #pragma omp parallel for collapse(2) schedule(dynamic)
    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            float x = ((i + 0.5) / width) - 0.5;
            float y = ((height - 1 - j + 0.5) / height) - 0.5;

            glm::vec4 uvk(U * x, V * y, -k, 1.f);

            glm::vec4 raydir = glm::normalize((uvk - eye));
            glm::vec4 worldRayDir = glm::normalize(camera.getViewMatrixInverse() * raydir); // to world space

            // Assign the computed color to the pixel
            Image[j * width + i] = raymarchCloud(glm::vec3(worldRayDir), glm::vec3(raydir), cloud, lightDir, lightColor, backgroundColor);

        }
    }

    // Save the image after processing
    
    if(saveImage(Image)){
        std::cout<< "finished" <<std::endl;
    }
    
    return 0;
}