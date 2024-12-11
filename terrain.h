#ifndef TERRAIN_H
#define TERRAIN_H


#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "camera.h"
#include "src/rgba.h"
#include "src/backend.h"

// Constants
static const float kMaxHeight = 840.0f;
static const float kMaxTreeHeight = 4.8f;

// Hash and Noise Functions
float hash1(glm::vec2 p);
float noise(glm::vec2 x);
float fbm(glm::vec2 x, int octaves, float frequency, float persistence);

// Terrain Map Functions
glm::vec2 terrainMap(glm::vec2 p, float baseY);
glm::vec4 terrainMapD(glm::vec2 p, float baseY);
glm::vec3 terrainNormal(glm::vec2 pos, float baseY);

// Raymarching Functions
glm::vec2 raymarchTerrain(glm::vec3 ro, glm::vec3 rd, float tmin, float tmax, float baseY);

// Rendering Function
glm::vec3 ReturnMountains(glm::vec3 eyePos, glm::vec3 wDir, glm::vec3 backgroundColorSky, float baseY);

#endif // TERRAIN_H