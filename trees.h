#ifndef TREES_H
#define TREES_H

#include "glm/glm.hpp"

float treesMap(glm::vec3 p, float rt, float &oHei, float &oMat, float &oDis);
float treesShadow(glm::vec3 ro, glm::vec3 rd);
glm::vec3 treesNormal(glm::vec3 pos, float t);

#endif