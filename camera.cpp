#include "camera.h"

Camera::Camera() {}

glm::mat4 Camera::getViewMatrix()
{

    // Translation matrix
    glm::mat4 M_translate(1.0f, 0.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f, 0.0f,
                          0.0f, 0.0f, 1.0f, 0.0f,
                          -pos[0], -pos[1], -pos[2], 1.0f);

    // Calculate w (forward) vector
    float lookLength = glm::length(look);
    if (lookLength < 1e-6f) return glm::mat4(1.0f); // Guard against zero length
    glm::vec4 w = -look / lookLength;

    // Calculate v (up) vector
    glm::vec4 upProjW = glm::dot(up, w) * w;
    glm::vec4 upPerp = up - upProjW;
    float upPerpLength = glm::length(upPerp);
    if (upPerpLength < 1e-6f) return glm::mat4(1.0f); // Guard against zero length
    glm::vec4 v = upPerp / upPerpLength;

    // Calculate u (right) vector using cross product
    glm::vec3 v3(v), w3(w);
    glm::vec3 u3 = glm::cross(v3, w3);
    glm::vec4 u = glm::vec4(u3, 0.0f);

    // Rotation matrix
    glm::mat4 M_rotate(u[0], v[0], w[0], 0.0f,
                       u[1], v[1], w[1], 0.0f,
                       u[2], v[2], w[2], 0.0f,
                       0.0f, 0.0f, 0.0f, 1.0f);

    return M_rotate * M_translate;
}

glm::mat4 Camera::getViewMatrixInverse()
{
    return glm::inverse(getViewMatrix());
}


