#ifndef NOISE_H
#define NOISE_H

#include "glm/glm.hpp"

float hash1(glm::vec2 p);
float hash1(float n);
glm::vec2 hash2(glm::vec2 p);
float noise1(glm::vec2 x);
float fbm_92(glm::vec2 x);
float fbm_42(glm::vec2 x);

#endif