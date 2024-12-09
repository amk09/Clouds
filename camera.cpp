#include "camera.h"
#include "glm/gtc/matrix_transform.hpp"
Camera::Camera() {}

glm::mat4 Camera::getViewMatrix() {
    glm::vec3 eye = glm::vec3(pos);
    glm::vec3 target = glm::vec3(pos + look); // Where the camera is looking
    glm::vec3 upDir = glm::vec3(up);

    // Create the view matrix using glm::lookAt
    return glm::lookAt(eye, target, upDir);
}

glm::mat4 Camera::getViewMatrixInverse()
{
    return glm::inverse(getViewMatrix());
}


