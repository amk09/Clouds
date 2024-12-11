#include "terrain_and_trees.h"
#include "config.h"          // 包含kMaxHeight,kMaxTreeHeight等
#include "terrain.h"         // raymarchTerrain, terrainNormal, terrainShadow
#include "trees.h"           // treesMap, treesShadow, treesNormal
#include "noise.h"           // fbm_4
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>

float cloudsShadowFlat(glm::vec3 ro, glm::vec3 rd);
extern glm::vec4 fbmd_7(glm::vec3 x);

glm::vec3 computeTerrainAndTreesColor(
    const glm::vec3 &eyePos, 
    const glm::vec3 &wDir, 
    const glm::vec3 &backgroundColorSky,
    const glm::vec3 &lightPos,
    const glm::vec3 &lightColor,
    float &resTOut,
    int &objOut
) {
    float tmin=0.0f,tmax=2000.0f;
    glm::vec2 tRes=raymarchTerrain(eyePos,wDir,tmin,tmax);
    bool hit=(tRes.x>0.0f);

    float resT=2000.0f;
    int obj=0;
    if(hit) {
        resT=tRes.x;
        obj=1;
    }

    float hei=0.f,mid=0.f,displa=0.f;
    if(tRes.y>0.0f){
        float tf=tRes.y;
        float tfMax=(tRes.x>0.0f)?tRes.x:tmax;
        for(int ii=0;ii<64;ii++){
            glm::vec3 p2=eyePos+wDir*tf;
            float dis=treesMap(p2,tf,hei,mid,displa);
            if(dis<(0.000125f*tf)) break;
            tf+=dis;
            if(tf>tfMax) break;
        }
        if(tf<tfMax){
            resT=tf;
            obj=2;
        }
    }

    glm::vec3 finalColor_terrain = backgroundColorSky;
    if(obj>0) {
        glm::vec3 pos=eyePos+wDir*resT;
        glm::vec3 epos=pos+glm::vec3(0.0f,4.8f,0.0f);

        // 光源方向：从表面点指向光源
        glm::vec3 L = glm::normalize(lightPos - pos);

        float sha1=terrainShadow(pos+glm::vec3(0,0.02f,0),L,0.02f);
        sha1*=glm::smoothstep(-0.325f,-0.075f,cloudsShadowFlat(epos,L));
        float sha2=1.0f;
        if(obj==2) sha2=treesShadow(pos+glm::vec3(0,0.02f,0),L);

        glm::vec3 tnor=terrainNormal(glm::vec2(pos.x,pos.z));
        glm::vec3 nor;
        glm::vec3 speC(1.0f);

        if(obj==1){
            // 地形着色
            nor=tnor;
            glm::vec3 col(0.18f,0.12f,0.10f);
            col*=0.85f;
            col=glm::mix(col,glm::vec3(0.1f,0.1f,0.0f)*0.2f,glm::smoothstep(0.7f,0.9f,nor.y));
            float dif=glm::clamp(glm::dot(nor,L),0.0f,1.0f);
            dif*=sha1;
            dif*=sha2;

            float bac=glm::clamp(glm::dot(glm::normalize(glm::vec3(-L.x,0.0f,-L.z)),nor),0.0f,1.0f);
            float foc=glm::clamp((pos.y/2.0f-180.0f)/130.0f,0.0f,1.0f);
            float dom=glm::clamp(0.5f+0.5f*nor.y,0.0f,1.0f);

            glm::vec3 lin=0.2f*glm::mix(0.1f*glm::vec3(0.1f,0.2f,0.1f),glm::vec3(0.7f,0.9f,1.5f)*3.0f,dom)*foc;
            lin+=8.5f*glm::vec3(1.0f,0.9f,0.8f)*dif;
            lin+=0.27f*glm::vec3(1.1f,1.0f,0.9f)*bac*foc;
            speC=glm::vec3(4.0f)*dif*glm::smoothstep(20.0f,0.0f,glm::abs(pos.y/2.0f-310.0f)-20.0f);

            // 将光的颜色叠加进去
            lin *= lightColor;

            col*=lin;
            finalColor_terrain=col;
        }else if(obj==2){
            // 树木着色
            glm::vec3 gnor=treesNormal(pos,resT);
            nor=glm::normalize(gnor+2.0f*tnor);

            float dif=glm::clamp(0.1f+0.9f*glm::dot(nor,L),0.0f,1.0f);
            dif*=sha1;
            if(dif>0.0001f){
                float a=glm::clamp(0.5f+0.5f*glm::dot(tnor,L),0.0f,1.0f);
                a=a*a;
                a*=glm::clamp(hei,0.0f,1.0f)*glm::pow(1.0f-2.0f*displa,3.0f);
                a*=0.6f;
                a*=glm::smoothstep(60.0f,200.0f,resT);
                dif*=a+(1.0f-a)*sha2;
            }

            float dom=glm::clamp(0.5f+0.5f*nor.y,0.0f,1.0f);
            float bac=glm::clamp(0.5f+0.5f*glm::dot(glm::normalize(glm::vec3(-L.x,0.0f,-L.z)),nor),0.0f,1.0f);
            float fre=glm::clamp(1.0f+glm::dot(nor,wDir),0.0f,1.0f);

            float brownAreas=fbm_4(glm::vec2(pos.z,pos.x)*0.015f);
            glm::vec3 col(0.2f,0.2f,0.05f);
            col=glm::mix(col,glm::vec3(0.32f,0.2f,0.05f),glm::smoothstep(0.2f,0.9f,glm::fract(2.0f*mid)));
            if(mid<0.5f) col*=0.65f+0.35f*glm::smoothstep(300.0f,600.0f,resT)*glm::smoothstep(700.0f,500.0f,pos.y);
            col=glm::mix(col,glm::vec3(0.25f,0.16f,0.01f)*0.825f,0.7f*glm::smoothstep(0.1f,0.3f,brownAreas)*glm::smoothstep(0.5f,0.8f,tnor.y));
            col*=1.0f-0.5f*glm::smoothstep(400.0f,700.0f,pos.y);

            glm::vec3 lin=12.0f*glm::vec3(1.2f,1.0f,0.7f)*dif*(2.5f-1.5f*glm::smoothstep(0.0f,120.0f,resT));
            lin+=0.55f*glm::mix(0.1f*glm::vec3(0.1f,0.2f,0.0f),glm::vec3(0.6f,1.0f,1.0f),dom*glm::clamp(hei,0.0f,1.0f)*glm::pow(1.0f-2.0f*displa,3.0f));
            lin+=0.07f*glm::vec3(1.0f,1.0f,0.9f)*bac*glm::clamp(hei,0.0f,1.0f)*glm::pow(1.0f-2.0f*displa,3.0f);
            lin+=1.10f*glm::vec3(0.9f,1.0f,0.8f)*glm::pow(fre,5.0f)*glm::clamp(hei,0.0f,1.0f)*(1.0f-glm::smoothstep(100.0f,200.0f,resT));

            
            lin *= lightColor;

            col*=lin;

            finalColor_terrain=col;
        }
    }

    resTOut=resT;
    objOut=obj;
    return finalColor_terrain;
}