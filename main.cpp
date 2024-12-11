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

#include "config.h"
#include "noise.h"
#include "terrain.h"
#include "trees.h"
#include "terrain_and_trees.h" 

float cloudsShadowFlat(glm::vec3 ro, glm::vec3 rd) {
    return 1.0f; // stub no clouds
}
 glm::vec4 fbmd_7(glm::vec3 x) {
    return glm::vec4(0,0,0,0); 
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
    camera.pos = glm::vec4(0.0f, 0.f, 0.0f, 1.0f); 
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
    glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f);

    // for (int frame = 0; frame < frames_to_render; ++frame) 
    for (int frame = 0; frame < frames_to_render; ++frame) 
    { 
        glm::vec3 cloudCenter(0.f, 3.f, -10.0f); 
        Cloud cloud1(cloudCenter, length1, breadth1, h1, densityOffset, shapeOffset , densityMultiplier, lightAbsorption);
        cloud1.shapeOffset.x += 0.1f; // Updating cloud positions

        glm::vec3 cloudCenter2(0.f, 0.0f, -20.0f); 
        Cloud cloud2(cloudCenter2, length, breadth, h, densityOffset * 8, shapeOffset , densityMultiplier * 2, lightAbsorption * 2);
        cloud2.shapeOffset.x += 1.f; // Updating cloud positions
        
        // lightPos = initialPosition + t * frame * movDir;
        // Light light1(lightPos, ligthColor, radius);
        
        
        std::vector<Light> l = lights(5, glm::vec3(0.f,0.f,-20.f), 10.f, off,rotationAxis);
        off += .05f;


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
                float resT;
                int obj;
                glm::vec3 finalColor_terrain = computeTerrainAndTreesColor(
                    glm::vec3(camera.pos), glm::vec3(worldRayDir), 
                    backgroundColor,
                    l[0].pos, l[0].emissionColor,
                    resT,
                    obj
                );
                
                //new addition ends

                glm::vec3 cloudDisplay1 = cloud1.renderClouds(glm::vec3(worldEye), glm::vec3(worldRayDir), backgroundColor, l);
                glm::vec3 cloudDisplay2 = cloud2.renderClouds(glm::vec3(worldEye), glm::vec3(worldRayDir), backgroundColor, l);
                //glm::vec3 lightDisplay = light1.lightSphereWithGlow(worldRayDir, camera.pos);
                //glm::vec3 surfColor = implicitPlaneIntersectWithLights(glm::vec3(worldEye), glm::vec3(worldRayDir),-5.f, l , glm::vec3(1.f,1.f,1.f));
                glm::vec3 surfColor = proceduralMountain(glm::vec3(worldEye), glm::vec3(worldRayDir),l,glm::vec3(1.f,1.f,1.f),20.f,2.f,.3f,-5.f);
                //glm::vec3 surfColor = implicitPlaneIntersect(glm::vec3(worldEye), glm::vec3(worldRayDir),-5.f, light1.pos, light1.emissionColor, glm::vec3(1.f,1.f,1.f));
                //glm::vec3 surfColor = implicitWavySurfaceIntersect(glm::vec3(worldEye), glm::vec3(worldRayDir), -5.f, light1.pos, light1.emissionColor, glm::vec3(1.f,1.f,1.f));
                Image[j * width + i] = convertVec3RGBA(cloudDisplay2  + disp + finalColor_terrain);
            }
        }

        // Save the frame

        std::string filename = "demo5/cloud_frame_" + std::to_string(frame) + ".png";
        saveImage(Image, filename.c_str(), width, height);
        std::cout << "Saved frame: " << filename << std::endl;
    }


    return 0;
}
