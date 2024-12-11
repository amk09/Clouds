#include "trees.h"
#include "noise.h"
#include "config.h"
#include "glm/glm.hpp"
#include"terrain.h"
#include "glm/gtc/noise.hpp"
#include "glm/gtx/transform.hpp"
#include <cmath>



// sdEllipsoidY在terrain中已定义，如果需要在trees中使用，请在trees.cpp中声明extern或复制
extern float sdEllipsoidY(glm::vec3 p, glm::vec2 r); 
// 在之前的代码中sdEllipsoidY定在main文件里，现在如果独立需要复制该函数：
static float sdEllipsoidYLocal(glm::vec3 p, glm::vec2 r) {
    glm::vec3 denom1(r.x,r.y,r.x);
    glm::vec3 denom2(r.x*r.x,r.y*r.y,r.x*r.x);
    float k0=glm::length(p/denom1);
    float k1=glm::length(p/denom2);
    return k0*(k0-1.0f)/k1;
}

// 若不想重复，可将sdEllipsoidY函数移动到单独common.h/common.cpp中，这里先重复：
float sdEllipsoidY(glm::vec3 p, glm::vec2 r) {
    return sdEllipsoidYLocal(p,r);
}

float treesMap(glm::vec3 p, float rt, float &oHei, float &oMat, float &oDis)
{
    oHei=1.0f;
    oDis=0.0f;
    oMat=0.0f;

    glm::vec2 P2(p.x,p.z);
    float base = terrainMap(P2).x;
    float bb = fbm_42(P2*0.075f);

    float d=20.0f;
    glm::vec2 n=glm::floor(P2/2.0f);
    glm::vec2 f=glm::fract(P2/2.0f);
    for(int j=0;j<=1;j++){
        for(int i=0;i<=1;i++){
            glm::vec2 g((float)i,(float)j);
            glm::vec2 s=glm::step(glm::vec2(0.5f,0.5f),f);
            g = g - s;

            glm::vec2 o = hash2(n+g);
            glm::vec2 shift(13.1f,71.7f);
            glm::vec2 v = hash2(n+g+shift);

            glm::vec2 r = g - f + o;

            float height = kMaxTreeHeight*(0.4f+0.8f*v.x);
            float width = 0.5f+0.2f*v.x+0.3f*v.y;

            if(bb<0.0f) width*=0.5f; else height*=0.7f;

            glm::vec3 q(r.x,p.y-base-height*0.5f,r.y);
            float k = sdEllipsoidY(q,glm::vec2(width,0.5f*height));

            if(k<d) {
                d=k;
                glm::vec2 shift2(111.0f,111.0f);
                oMat=0.5f*hash1(n+g+shift2);
                if(bb>0.0f) oMat+=0.5f;
                oHei=(p.y-base)/height;
                oHei*=0.5f+0.5f*glm::length(q)/width;
            }
        }
    }

    if(rt<1200.0f){
        glm::vec3 pp=p;
        pp.y-=600.0f;
        float s=fbm_42(glm::vec2(pp.x,pp.z)*3.0f);
        s=s*s;
        float att=1.0f - glm::smoothstep(100.0f,1200.0f,rt);
        d+=4.0f*s*att;
        oDis=s*att;
    }

    return d;
}

float treesShadow(glm::vec3 ro, glm::vec3 rd)
{
    float res=1.0f;
    float t=0.02f;
    for(int i=0;i<150;i++)
    {
        float kk1,kk2,kk3;
        glm::vec3 pos=ro+rd*t;
        float h=treesMap(pos,t,kk1,kk2,kk3);
        res=glm::min(res,32.0f*h/t);
        t+=h;
        if(res<0.001f||t>120.0f||pos.y>kMaxHeight+kMaxTreeHeight) break;
    }
    return glm::clamp(res,0.0f,1.0f);
}

glm::vec3 treesNormal(glm::vec3 pos, float t)
{
    float kk1,kk2,kk3;
    glm::vec3 dirs[4] = {
        glm::vec3(0.5773f,-0.5773f,-0.5773f),
        glm::vec3(-0.5773f,-0.5773f,0.5773f),
        glm::vec3(-0.5773f,0.5773f,-0.5773f),
        glm::vec3(0.5773f,0.5773f,0.5773f)
    };

    glm::vec3 n(0.0f);
    for(int i=0;i<4;i++){
        glm::vec3 e=dirs[i];
        float h=treesMap(pos+0.005f*e,t,kk1,kk2,kk3);
        n+=e*h;
    }
    return glm::normalize(n);
}