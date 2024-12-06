#include "src/rgba.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"


RGBA convertVec3RGBA(glm::vec3 vector)
{
    // Clamp each component to the range [0, 1]
    vector = glm::clamp(vector, 0.0f, 1.0f);

    // Scale components to the range [0, 255]
    int r = static_cast<int>(vector.r * 255.0f);
    int g = static_cast<int>(vector.g * 255.0f);
    int b = static_cast<int>(vector.b * 255.0f);

    return RGBA(r, g, b, 255);
}