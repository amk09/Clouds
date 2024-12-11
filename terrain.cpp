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
inline float hash1(glm::vec2 p) {
    p = 50.0f * glm::fract(p * 0.3183099f);
    return glm::fract(p.x * p.y * (p.x + p.y));
}

inline float noise(glm::vec2 x) {
    glm::vec2 P = glm::floor(x);
    glm::vec2 w = glm::fract(x);
    glm::vec2 u = w * w * w * (w * (w * 6.0f - 15.0f) + 10.0f);

    float a = hash1(P + glm::vec2(0, 0));
    float b = hash1(P + glm::vec2(1, 0));
    float c = hash1(P + glm::vec2(0, 1));
    float d = hash1(P + glm::vec2(1, 1));

    return -1.0f + 2.0f * (a + (b - a) * u.x + (c - a) * u.y + (a - b - c + d) * u.x * u.y);
}

inline float fbm(glm::vec2 x, int octaves, float frequency, float persistence) {
    float amplitude = 1.0f;
    float gain = 0.5f;
    float result = 0.0f;

    for (int i = 0; i < octaves; ++i) {
        result += amplitude * noise(x * frequency);
        x *= 1.9f;
        amplitude *= persistence;
    }
    return result;
}

// Terrain Map Functions
inline glm::vec2 terrainMap(glm::vec2 p, float baseY) {
    float e = fbm(p / 2000.0f, 9, 1.9f, 0.55f);
    float a = 1.0f - glm::smoothstep(0.12f, 0.13f, glm::abs(e + 0.12f));
    e = 600.0f * e + baseY;
    e += 90.0f * glm::smoothstep(baseY + 52.0f, baseY + 94.0f, e);
    return glm::vec2(e, a);
}

inline glm::vec4 terrainMapD(glm::vec2 p, float baseY) {
    float eps = 0.1f;
    float h = terrainMap(p, baseY).x;
    float hx = (terrainMap(p + glm::vec2(eps, 0), baseY).x - terrainMap(p - glm::vec2(eps, 0), baseY).x) / (2 * eps);
    float hz = (terrainMap(p + glm::vec2(0, eps), baseY).x - terrainMap(p - glm::vec2(0, eps), baseY).x) / (2 * eps);
    glm::vec3 N = glm::normalize(glm::vec3(-hx, 1.0f, -hz));
    return glm::vec4(h, N);
}

inline glm::vec3 terrainNormal(glm::vec2 pos, float baseY) {
    glm::vec4 tmp = terrainMapD(pos, baseY);
    return glm::vec3(tmp.y, tmp.z, tmp.w);
}

// Raymarching Functions
inline glm::vec2 raymarchTerrain(glm::vec3 ro, glm::vec3 rd, float tmin, float tmax, float baseY) {
    float tp = (baseY + kMaxHeight - ro.y) / rd.y;
    if (tp > 0.0f) tmax = glm::min(tmax, tp);

    float dis, th;
    float t2 = -1.0f;
    float t = tmin;
    float ot = t;
    float odis = 0.0f;
    float odis2 = 0.0f;

    for (int i = 0; i < 400; i++) {
        th = 0.001f * t;
        glm::vec3 pos = ro + rd * t;
        glm::vec2 env = terrainMap(glm::vec2(pos.x, pos.z), baseY);
        float hei = pos.y - env.x;

        float dis2 = pos.y - (env.x + kMaxTreeHeight * 1.1f);
        if (dis2 < th) {
            if (t2 < 0.0f) {
                t2 = ot + (th - odis2) * (t - ot) / (dis2 - odis2);
            }
        }
        odis2 = dis2;

        dis = pos.y - env.x;
        if (dis < th) break;

        ot = t;
        odis = dis;
        t += dis * 0.8f * (1.0f - 0.75f * env.y);
        if (t > tmax) break;
    }

    if (t > tmax) t = -1.0f;
    else t = ot + (th - odis) * (t - ot) / (dis - odis);

    return glm::vec2(t, t2);
}

// Rendering Function
inline glm::vec3 ReturnMountains(glm::vec3 eyePos, glm::vec3 wDir, glm::vec3 backgroundColorSky, float baseY) {
    float tmin = 0.0f, tmax = 2000.0f;
    glm::vec2 tRes = raymarchTerrain(eyePos, wDir, tmin, tmax, baseY);
    if (tRes.x <= 0.0f) return backgroundColorSky;

    glm::vec3 pos = eyePos + tRes.x * wDir;
    glm::vec3 normal = terrainNormal(glm::vec2(pos.x, pos.z), baseY);
    //return glm::mix(glm::vec3(0.5f, 0.3f, 0.1f), glm::vec3(1.0f), glm::dot(normal, glm::vec3(0, 1, 0)));
    return glm::vec3(1.0f, 0.5f, 0.3f);
}
#endif // TERRAIN_H
