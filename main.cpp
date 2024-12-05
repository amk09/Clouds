#include "glm/gtx/transform.hpp"
#include <iostream>
#include "camera.h"
#include <iostream>
#include "src/rgba.h"
#include "src/backend.h"
#include "Cloud.h"

int main()
{
    Camera camera;

    int height = 1000, width = 1000;
    std::vector<RGBA> Image(width * height, RGBA(0, 0, 0, 255));

    float k = .1f;
    float horizontal_angle = 30.0;
    float V = 2 * k * glm::tan(horizontal_angle / 2);
    float U = (width * V) / height;

    glm::vec4 eye(0.f, 0.f, 0.f, 1.f);
    glm::vec4 worldEye = camera.getViewMatrixInverse() * eye;

    for (int j = 0; j < height; j++)
        for (int i = 0; i < width; i++)
        {
            float x = ((i + 0.5) / width) - 0.5;
            float y = ((height - 1 - j + 0.5) / height) - 0.5;

            glm::vec4 uvk(U * x, V * y, -k, 1.f);

            glm::vec4 raydir = glm::normalize((uvk - eye));
            glm::vec4 worldRayDir = glm::normalize(camera.getViewMatrixInverse() * raydir); // to world space
            Image[j * width + i] = Color(worldEye, worldRayDir);

            // if (i == j)
            //     Image[j * width + i] = RGBA(255, 255, 255, 255);
        }
    saveImage(Image);
}