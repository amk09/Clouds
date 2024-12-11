#ifndef TERRAIN_H
#define TERRAIN_H

#include "glm/glm.hpp"

glm::vec2 terrainMap(glm::vec2 p);
glm::vec4 terrainMapD(glm::vec2 p);
glm::vec3 terrainNormal(glm::vec2 pos);
float terrainShadow(glm::vec3 ro, glm::vec3 rd, float mint);
glm::vec2 raymarchTerrain(glm::vec3 ro, glm::vec3 rd, float tmin, float tmax);

#endif