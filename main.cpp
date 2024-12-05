#include "glm/gtx/transform.hpp"
#include "camera.h"
#include <iostream>

int main()
{
    Camera c;
    glm::vec3 x = glm::vec3(1.f);
    std::cout<<c.getViewMatrix()[1][1];
}
