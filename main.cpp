#include <omp.h>
#include <cstdio>
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include <iostream>

#include "camera.h"
#include "src/rgba.h"
#include "src/backend.h"
#include "Cloud.h"


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




int main(int argc, char* argv[])
{
    // int frameBegin;
    // int frameEnd;
    int seconds;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <int1> \n";
        return 1;
    }

    try {
     
        // frameBegin = std::stoi(argv[1]); 
        // frameEnd = std::stoi(argv[2]); 
        seconds = std::stoi(argv[1]); 

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Please provide valid integers.\n";
        return 1;
    } catch (const std::out_of_range& e) {
        std::cerr << "Error: Integer out of range.\n";
        return 1;
    }

    // Output setup     
    int height = 480;
    int width = 640;

    glm::vec3 backgroundColor(0.f,0.f,0.f); //(0.5f, 0.7f, 1.0f); 
    std::vector<RGBA> Image(width * height, RGBA(0, 0, 0, 255));

    // Light setup

    glm::vec3 lightPos(0.f,0.0f,-10.f);
    glm::vec3 ligthColor(1.f,1.f,1.f);
    float radius(0.2f);

    //Light light1(lightPos, ligthColor, radius);

    // Camera setup

    Camera camera;
    camera.pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); 
    camera.look = glm::normalize(glm::vec4(0.f,0.f,-1.f,0.f)); 
    camera.up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); 

    float k = 0.1f;
    float horizontal_angle = 45.0;
    float V = 2 * k * glm::tan(glm::radians(horizontal_angle / 2));
    float U = (width * V) / height;

    glm::vec4 worldEye = camera.pos;


    // Clouds setup 
    float length1 = 10.f;                    // Lenght  is along the x axis
    float breadth1 = 1.5f;                   // Breadth is along the y axis 
    float h1 = 10.f;    
    
    float length = 1.5f;                    // Lenght  is along the x axis
    float breadth = 1.5f;                   // Breadth is along the y axis 
    float h = 1.5f;                        // Height  is along the z axis
    float densityOffset = 0.1f;             // Changes the density of cloud, less is more dense
    float densityMultiplier = 0.2f;          // Increasing would increase density
    float lightAbsorption = 0.1f;           // Increasing would darken the clouds  ; ideas : can tweak this value to make rain
    glm::vec3 shapeOffset(0.f, 0.f, 0.f);   // Movement x,y,z for directional movements

    // Define start and end positions for the light
    // glm::vec3 startPos = glm::vec3(0.f,0.-0.7f,0.f);   // at camera
    // glm::vec3 endPos   = glm::vec3(0.f,.3f,0.f); // well beyond the cloud center
    // glm::vec3 dir = glm::normalize(endPos - startPos);
    // int totalFrames = 24;
    // glm::vec3 red(1.0f, 1.0f, 1.0f);
    // glm::vec3 orange(1.0f, 1.f, 1.0f);
    // glm::vec3 white(1.0f, 1.0f, 1.0f);

    omp_set_num_threads(omp_get_max_threads());
    float time = 0.0f; // Initialize time
    //int seconds = 15;
    int frames_to_render =  seconds * 24;
    float x = 10.f;

    glm::vec3 initialPosition = glm::vec3(0.0f,10.0f,-20.f);
    glm::vec3 finalPosition = glm::vec3(0.0f,0.0f,-10.f);   
    glm::vec3 movDir = glm::normalize(finalPosition - initialPosition);
    float distance = glm::length(finalPosition - initialPosition);
    float t = distance / static_cast<float>(frames_to_render);
    float off = 0.f;
    glm::vec3 rotationAxis(0.0f, 0.0f, 1.0f);

    // for (int frame = 0; frame < frames_to_render; ++frame) 
    for (int frame = 0; frame < frames_to_render; ++frame) 
    { 
        glm::vec3 cloudCenter(0.f, 3.f, -10.0f); 
        Cloud cloud1(cloudCenter, length1, breadth1, h1, densityOffset, shapeOffset , densityMultiplier, lightAbsorption);
        cloud1.shapeOffset.x += 0.1f; // Updating cloud positions

        glm::vec3 cloudCenter2(0.f, 0.0f, -20.0f); 
        Cloud cloud2(cloudCenter2, length, breadth, h, densityOffset * 8, shapeOffset , densityMultiplier * 2, lightAbsorption * 2);
        cloud2.shapeOffset.x += 0.2f; // Updating cloud positions
        
        // lightPos = initialPosition + t * frame * movDir;
        // Light light1(lightPos, ligthColor, radius);
        
        
        std::vector<Light> l = lights(5, glm::vec3(0.f,0.f,-20.f), 2.f, off,rotationAxis);
        off += .1f;


        // OpenMP parallel loop for rendering
        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (int j = 0; j < height; j++) 
        {
            for (int i = 0; i < width; i++) 
            {

                float x = ((i + 0.5f) / width) - 0.5f;
                float y = ((height - 1 - j + 0.5f) / height) - 0.5f;

                // Shooting rays from center to viewport
                glm::vec4 uvk(U * x, V * y, -k, 1.f);
                glm::vec4 raydir = glm::normalize((uvk - camera.pos));
                glm::vec4 worldRayDir = glm::normalize(camera.getViewMatrixInverse() * raydir); 

                glm::vec3 disp(0.f,0.f,0.f);
                for(auto x : l)
                    disp += x.lightSphereWithGlow(worldRayDir, camera.pos);

                // new addition
                
                //new addition ends

                glm::vec3 cloudDisplay1 = cloud1.renderClouds(glm::vec3(worldEye), glm::vec3(worldRayDir), backgroundColor, l);
                glm::vec3 cloudDisplay2 = cloud2.renderClouds(glm::vec3(worldEye), glm::vec3(worldRayDir), backgroundColor, l);
                //glm::vec3 lightDisplay = light1.lightSphereWithGlow(worldRayDir, camera.pos);
                //glm::vec3 surfColor = implicitPlaneIntersectWithLights(glm::vec3(worldEye), glm::vec3(worldRayDir),-5.f, l , glm::vec3(1.f,1.f,1.f));
                //glm::vec3 surfColor = implicitPlaneIntersect(glm::vec3(worldEye), glm::vec3(worldRayDir),-5.f, light1.pos, light1.emissionColor, glm::vec3(1.f,1.f,1.f));
                //glm::vec3 surfColor = implicitWavySurfaceIntersect(glm::vec3(worldEye), glm::vec3(worldRayDir), -5.f, light1.pos, light1.emissionColor, glm::vec3(1.f,1.f,1.f));
                Image[j * width + i] = convertVec3RGBA(cloudDisplay2 + cloudDisplay1 + surfColor + disp);
            }
        }

        // Save the frame

        std::string filename = "demo4/cloud_frame_" + std::to_string(frame) + ".png";
        saveImage(Image, filename.c_str(), width, height);
        std::cout << "Saved frame: " << filename << std::endl;
    }


    return 0;
}
