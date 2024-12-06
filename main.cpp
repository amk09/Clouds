#include "glm/gtx/transform.hpp"
#include <iostream>
#include <vector>
#include "camera.h"
#include "src/rgba.h"
#include "src/backend.h"
#include "Cloud.h"

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
            Image[j * width + i] = Color(worldEye, worldRayDir);
        }
    }

    // Save the image after processing
    saveImage(Image);

    return 0;
}