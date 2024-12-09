#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"

class Camera
{
public:
    Camera();
    glm::vec4 pos;
    glm::vec4 look;
    glm::vec4 up;

    glm::mat4 getViewMatrix();

    glm::mat4 getViewMatrixInverse();
};

#endif // CAMERA_H
