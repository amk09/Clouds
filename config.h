#ifndef CONFIG_H
#define CONFIG_H

#include "glm/glm.hpp"

#define iFrame 0
#define ZERO (glm::min(iFrame,0))

extern float kMaxHeight;
extern float kMaxTreeHeight;

// 全局光照方向（如果需要外部访问）
extern glm::vec3 kSunDir;

#endif