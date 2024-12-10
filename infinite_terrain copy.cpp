#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdio>
#include "glm/glm.hpp"
#include "glm/gtc/noise.hpp"
#include "glm/gtx/transform.hpp"

//-----------------------------------------
// 你需要的宏定义
//-----------------------------------------
#define iFrame 0
#define ZERO (std::min(iFrame,0))
static float kMaxHeight = 840.0f;
static float kMaxTreeHeight = 4.8f;

//-----------------------------------------
// 简易颜色和图像保存（伪实现）
// 请根据你自己的函数实现
//-----------------------------------------
struct RGBA {
    unsigned char r,g,b,a;
    RGBA(unsigned char R=0,unsigned char G=0,unsigned char B=0,unsigned char A=255)
    :r(R),g(G),b(B),a(A){}
};

inline RGBA convertVec3RGBA(const glm::vec3 &c) {
    int R = (int)(255.0f*std::max(0.0f,std::min(1.0f,c.r)));
    int G = (int)(255.0f*std::max(0.0f,std::min(1.0f,c.g)));
    int B = (int)(255.0f*std::max(0.0f,std::min(1.0f,c.b)));
    return RGBA((unsigned char)R,(unsigned char)G,(unsigned char)B,255);
}

// 假设使用简单PPM格式保存
void saveImage(const std::vector<RGBA> &img, const char *filename, int width, int height) {
    std::ofstream ofs(filename,std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (auto &px : img) {
        ofs.put((char)px.r);
        ofs.put((char)px.g);
        ofs.put((char)px.b);
    }
    ofs.close();
}

//-----------------------------------------
// hash和噪声函数 (从你的代码中提取简化版)
//-----------------------------------------
static float hash1(glm::vec2 p) {
    p  = 50.0f*glm::fract(p*0.3183099f);
    return glm::fract(p.x*p.y*(p.x+p.y));
}

static float hash1(float n) {
    return glm::fract(n*17.0f*glm::fract(n*0.3183099f));
}

//-----------------------------------------
// noise函数需要：
// fbm_9, fbmd_9函数依赖m2, m2i
//-----------------------------------------
const glm::mat2 m2 = glm::mat2( 0.80f,  0.60f,
                               -0.60f,  0.80f );
const glm::mat2 m2i = glm::mat2( 0.80f, -0.60f,
                                 0.60f,  0.80f );

// 简单noise2D
float noise(glm::vec2 x) {
    glm::vec2 p = glm::floor(x);
    glm::vec2 w = glm::fract(x);
    glm::vec2 u = w*w*w*(w*(w*6.0f-15.0f)+10.0f);

    float a = hash1(p+glm::vec2(0,0));
    float b = hash1(p+glm::vec2(1,0));
    float c = hash1(p+glm::vec2(0,1));
    float d = hash1(p+glm::vec2(1,1));

    return -1.0f+2.0f*(a + (b - a)*u.x + (c - a)*u.y + (a - b - c + d)*u.x*u.y);
}

// fbm_9函数
float fbm_9( glm::vec2 x )
{
    float f = 1.9f;
    float s = 0.55f;
    float a = 0.0f;
    float b = 0.5f;
    for( int i=0; i<9; i++ )
    {
        float n = noise(x);
        a += b*n;
        b *= s;
        x = f*m2*x;
    }
    return a;
}

// smoothstepd函数
// 返回smoothstep及其导数
glm::vec2 smoothstepd(float a, float b, float x)
{
    if(x<a) return glm::vec2(0.0f,0.0f);
    if(x>b) return glm::vec2(1.0f,0.0f);
    float ir = 1.0f/(b-a);
    float t = (x - a)*ir;
    return glm::vec2(t*t*(3.0f - 2.0f*t), 6.0f*t*(1.0f-t)*ir);
}

//-----------------------------------------
// terrainMap / terrainMapD / terrainNormal / terrainShadow / raymarchTerrain
// 来自你的代码
//-----------------------------------------

glm::vec2 terrainMap( glm::vec2 p )
{
    float e = fbm_9( p/2000.0f + glm::vec2(1.0f,-2.0f) );
    float a = 1.0f - glm::smoothstep(0.12f,0.13f,fabs(e+0.12f));
    e = 600.0f*e + 600.0f;

    // cliff
    e += 90.0f*glm::smoothstep( 552.0f, 594.0f, e );

    return glm::vec2(e,a);
}

glm::vec4 terrainMapD( glm::vec2 p )
{
    // 此处原代码使用fbmd_9, 但未给出实现。
    // 简化处理：使用有限差分近似导数代替，保证独立运行

    float eps = 0.1f;
    float h = terrainMap(p).x;
    float hx = (terrainMap(p+glm::vec2(eps,0)).x - terrainMap(p-glm::vec2(eps,0)).x)/(2*eps);
    float hz = (terrainMap(p+glm::vec2(0,eps)).x - terrainMap(p-glm::vec2(0,eps)).x)/(2*eps);

    // 构造法线
    glm::vec3 N = glm::normalize(glm::vec3(-hx,1.0f,-hz));
    return glm::vec4(h,N);
}

glm::vec3 terrainNormal(glm::vec2 pos )
{
    glm::vec4 tmp = terrainMapD(pos);
    // tmp = (h, Nx, Ny, Nz)
    return glm::vec3(tmp.y, tmp.z, tmp.w);
}

float terrainShadow( glm::vec3 ro, glm::vec3 rd,  float mint )
{
    float res = 1.0f;
    float t = mint;
    for(int i=0; i<128; i++)
    {
        glm::vec3 pos = ro + rd*t;
        glm::vec2 env = terrainMap(glm::vec2(pos.x,pos.z)); // 替换 pos.xz -> glm::vec2(pos.x,pos.z)
        float hei = pos.y - env.x;
        res = std::min(res, 32.0f*hei/t);
        if( res<0.0001f || pos.y>kMaxHeight ) break;
        t += glm::clamp(hei, 0.5f+t*0.05f, 25.0f); // 使用glm::clamp
    }
    return glm::clamp(res, 0.0f, 1.0f);
}

glm::vec2 raymarchTerrain(glm::vec3 ro, glm::vec3 rd, float tmin, float tmax )
{
    float tp = (kMaxHeight+kMaxTreeHeight - ro.y)/rd.y;
    if(tp>0.0f) tmax = std::min(tmax,tp);

    float dis, th;
    float t2 = -1.0f;
    float t = tmin;
    float ot = t;
    float odis = 0.0f;
    float odis2 = 0.0f;
    for(int i=0; i<400; i++)
    {
        th = 0.001f*t;
        glm::vec3 pos = ro + rd*t;
        glm::vec2 env = terrainMap(glm::vec2(pos.x,pos.z)); // 同样替换 pos.xz
        float hei = env.x;

        float dis2 = pos.y - (hei+kMaxTreeHeight*1.1f);
        if(dis2<th) 
        {
            if(t2<0.0f)
            {
                t2 = ot + (th-odis2)*(t-ot)/(dis2-odis2);
            }
        }
        odis2 = dis2;

        dis = pos.y - hei;
        if(dis<th) break;

        ot = t;
        odis = dis;
        t += dis*0.8f*(1.0f - 0.75f*env.y);
        if(t>tmax) break;
    }

    if(t>tmax) t = -1.0f;
    else t = ot + (th-odis)*(t-ot)/(dis-odis);

    return glm::vec2(t,t2);
}

//-----------------------------------------
// 主函数：使用上述函数渲染地形
//-----------------------------------------
int main() {
    int width = 600;
    int height = 400;
    std::vector<RGBA> Image(width*height, RGBA(0,0,0,255));

    // 相机设置：在高处，向下倾斜
    glm::vec3 eyePos(0,500,500);
    glm::vec3 target(0,0,0);
    glm::vec3 up(0,1,0);

    glm::mat4 view = glm::lookAt(eyePos,target,up);
    glm::mat4 invView = glm::inverse(view);

    float fov = 60.0f;
    float k = 0.1f;
    float V = 2.0f*k*std::tan(glm::radians(fov/2.0f));
    float U = (width * V)/height;

    glm::vec3 backgroundColorSky(0.42f,0.62f,1.1f);
    glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f,0.5f,-0.5f));

    #pragma omp parallel for collapse(2)
    for (int j=0; j<height; j++) {
        for (int i=0; i<width; i++) {
            float x = ((i+0.5f)/width)-0.5f;
            float y = ((height-1 - j + 0.5f)/height)-0.5f;

            glm::vec4 rayCamDir(U*x, V*y, -k, 0.0f);
            glm::vec4 worldRayDir4 = invView * rayCamDir;
            glm::vec3 wDir = glm::normalize(glm::vec3(worldRayDir4));

            // 射线求交
            float tmin = 0.0f;
            float tmax = 2000.0f;
            glm::vec2 tRes = raymarchTerrain(eyePos, wDir, tmin, tmax);
            bool hit = (tRes.x>0.0f);

            glm::vec3 finalColor = backgroundColorSky;
            if(hit) {
                    float tHit = tRes.x;
                    glm::vec3 p = eyePos + wDir*tHit;

                    // 使用 glm::vec2(p.x, p.z) 代替 p.xz
                    float h = terrainMap(glm::vec2(p.x, p.z)).x;
                    glm::vec3 N = terrainNormal(glm::vec2(p.x, p.z));

                    // 按高度着色：低为深灰，高为白
                    glm::vec3 hitColor;
                    if (h > 300.0f) {
                        hitColor = glm::vec3(1.0f);
                    } else {
                        float tt = glm::clamp(h/300.0f, 0.0f, 1.0f);
                        hitColor = glm::mix(glm::vec3(0.0f), glm::vec3(0.5f), tt);
                    }

                    float dif = glm::max(0.0f, glm::dot(N, lightDir));
                    float ambient = 0.3f;
                    finalColor = hitColor * (ambient + dif * 0.7f);
                }

            Image[j*width + i] = convertVec3RGBA(finalColor);
        }
    }

    saveImage(Image,"terrain_output.ppm",width,height);
    std::cout << "Saved terrain_output.ppm\n";

    return 0;
}
