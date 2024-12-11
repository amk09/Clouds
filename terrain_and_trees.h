#ifndef TERRAIN_AND_TREES_H
#define TERRAIN_AND_TREES_H

#include "glm/glm.hpp"

// Declare external dependencies
extern glm::vec2 raymarchTerrain(glm::vec3 ro, glm::vec3 rd, float tmin, float tmax);
extern float terrainShadow(glm::vec3 ro, glm::vec3 rd, float mint);
extern glm::vec3 terrainNormal(glm::vec2 pos);
extern float treesMap(glm::vec3 p, float rt, float &oHei, float &oMat, float &oDis);
extern float treesShadow(glm::vec3 ro, glm::vec3 rd);
extern glm::vec3 treesNormal(glm::vec3 pos, float t);
extern float fbm_4(glm::vec2 x);

// Declare the main function
glm::vec3 computeTerrainAndTreesColor(
    const glm::vec3 &eyePos, 
    const glm::vec3 &wDir, 
    const glm::vec3 &backgroundColorSky,
    const glm::vec3 &lightPos,
    const glm::vec3 &lightColor,
    float &resTOut,
    int &objOut
);

#endif // TERRAIN_AND_TREES_H