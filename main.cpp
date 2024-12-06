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

    // Camera setup: Play with the camera to see 
    camera.pos = glm::vec4(30.0f, 20.0f, 30.0f, 1.0f); // Position the camera
    glm::vec3 cloudCenter(0.0f, -10.0f, 0.0f); 
    camera.look = glm::normalize(glm::vec4(cloudCenter, 1.0f) - camera.pos); // Look at the cloud box
    camera.up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // Keep the camera upright

    int height = 1000, width = 1000;
    std::vector<RGBA> Image(width * height, RGBA(0, 0, 0, 255));

    // Adjust frustum based on camera distance
    float k = glm::length(glm::vec3(camera.pos - glm::vec4(0.0f))); // Near plane distance
    float horizontal_angle = 45.0f;
    float V = 2 * k * glm::tan(glm::radians(horizontal_angle / 2));
    float U = (width * V) / height;

    glm::vec4 eye = camera.pos; // Use camera's position as the eye
    glm::vec4 worldEye = camera.getViewMatrixInverse() * eye;

    float length = 10.0f, breadth = 10.0f, h = 10.0f;
    float densityOffset = 0.1f, densityMultiplier = 0.8f, lightAbsorption = 0.1f;
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

            // Adjust uvk for the camera's distance
            glm::vec4 uvk(U * x, V * y, -k, 1.f);

            glm::vec4 raydir = glm::normalize((uvk - eye));
            glm::vec4 worldRayDir = glm::normalize(camera.getViewMatrixInverse() * raydir); // Convert to world space

            // Assign the computed color to the pixel
            Image[j * width + i] = Color(worldEye, worldRayDir) ;
        }
    }

    // Save the image after processing
    if (saveImage(Image)) {
        std::cout << "Rendering completed." << std::endl;
    }

    return 0;
}
