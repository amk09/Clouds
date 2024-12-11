#include "terrain.h"
#include "noise.h"
#include "config.h"
#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"
#include "glm/gtx/transform.hpp"


glm::vec2 terrainMap(glm::vec2 p)
{
    float e = fbm_92(p/2000.0f+glm::vec2(1.0f,-2.0f));
    float a = 1.0f - glm::smoothstep(0.12f,0.13f,glm::abs(e+0.12f));
    e = 600.0f*e + 600.0f;
    e += 90.0f*glm::smoothstep(552.0f,594.0f,e);
    return glm::vec2(e,a);
}

glm::vec4 terrainMapD(glm::vec2 p)
{
    float eps =0.1f;
    float h = terrainMap(p).x;
    float hx = (terrainMap(p+glm::vec2(eps,0)).x - terrainMap(p-glm::vec2(eps,0)).x)/(2*eps);
    float hz = (terrainMap(p+glm::vec2(0,eps)).x - terrainMap(p-glm::vec2(0,eps)).x)/(2*eps);

    glm::vec3 N=glm::normalize(glm::vec3(-hx,1.0f,-hz));
    return glm::vec4(h,N);
}

glm::vec3 terrainNormal(glm::vec2 pos)
{
    glm::vec4 tmp=terrainMapD(pos);
    return glm::vec3(tmp.y,tmp.z,tmp.w);
}

float terrainShadow(glm::vec3 ro, glm::vec3 rd, float mint)
{
    float res=1.0f;
    float t=mint;
    for(int i=0;i<128;i++)
    {
        glm::vec3 pos=ro+rd*t;
        glm::vec2 env=terrainMap(glm::vec2(pos.x,pos.z));
        float hei=pos.y - env.x;
        res=glm::min(res,32.0f*hei/t);
        if(res<0.0001f||pos.y>kMaxHeight) break;
        t+=glm::clamp(hei,0.5f+t*0.05f,25.0f);
    }
    return glm::clamp(res,0.0f,1.0f);
}

glm::vec2 raymarchTerrain(glm::vec3 ro, glm::vec3 rd, float tmin, float tmax )
{
    float tp=(kMaxHeight+kMaxTreeHeight - ro.y)/rd.y;
    if(tp>0.0f) tmax=glm::min(tmax,tp);

    float dis,th;
    float t2=-1.0f;
    float t=tmin;
    float ot=t;
    float odis=0.0f;
    float odis2=0.0f;
    for(int i=0;i<400;i++)
    {
        th=0.001f*t;
        glm::vec3 pos=ro+rd*t;
        glm::vec2 env=terrainMap(glm::vec2(pos.x,pos.z));
        float hei=env.x;

        float dis2=pos.y-(hei+kMaxTreeHeight*1.1f);
        if(dis2<th) {
            if(t2<0.0f){
                t2=ot+(th-odis2)*(t-ot)/(dis2-odis2);
            }
        }
        odis2=dis2;

        dis=pos.y - hei;
        if(dis<th) break;

        ot=t;
        odis=dis;
        t+=dis*0.8f*(1.0f-0.75f*env.y);
        if(t>tmax) break;
    }

    if(t>tmax) t=-1.0f;
    else t=ot+(th-odis)*(t-ot)/(dis-odis);

    return glm::vec2(t,t2);
}