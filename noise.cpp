#include "noise.h"
#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"
#include "Cloud.h"

static const glm::mat2 m2 = glm::mat2(0.80f,0.60f,-0.60f,0.80f);



glm::vec2 hash2(glm::vec2 p) {
    glm::vec2 k(0.3183099f,0.3678794f);
    float n=111.0f*p.x+113.0f*p.y;
    return glm::fract(n*glm::fract(k*n));
}

float noise1(glm::vec2 x) {
    glm::vec2 P = glm::floor(x);
    glm::vec2 w = glm::fract(x);
    glm::vec2 u = w*w*w*(w*(w*6.0f-15.0f)+10.0f);

    float a = hash1(P+glm::vec2(0,0));
    float b = hash1(P+glm::vec2(1,0));
    float c = hash1(P+glm::vec2(0,1));
    float d = hash1(P+glm::vec2(1,1));

    return -1.0f+2.0f*(a+(b-a)*u.x+(c-a)*u.y+(a-b-c+d)*u.x*u.y);
}

float fbm_92(glm::vec2 x) {
    float f=1.9f,s=0.55f;
    float a=0.0f,b=0.5f;
    for(int i=0;i<9;i++){
        a+=b*noise1(x);
        b*=s;
        x=f*m2*x;
    }
    return a;
}

float fbm_42(glm::vec2 x){
    float f=1.9f,s=0.55f;
    float a=0.0f,b=0.5f;
    for(int i=0;i<4;i++){
        a+=b*noise1(x);
        b*=s;
        x=f*m2*x;
    }
    return a;
}