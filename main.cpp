#include "glm/gtx/transform.hpp"
#include <iostream>
#include "src/rgba.h"
#include <iostream>
#include <algorithm>
#include <fstream>





int main()
{   
    // glm::vec3 x = glm::vec3(1.f);
    // std::cout<<x[1];
    Camera c;
    glm::vec3 x = glm::vec3(1.f);
    std::cout<<c.getViewMatrix()[1][1];
    const int width = 256;  
    const int height = 256; 

    
    std::vector<RGBA> m_data(width * height);

    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            
            float intensity = ((float)x / (width - 1)) * ((float)y / (height - 1)) * 255.0f;

            // Assign intensity to all color channels
            uint8_t value = static_cast<uint8_t>(std::round(intensity));
            m_data[y * width + x] = {value, value, value, 255}; 
        }
    }

   // Write the image to a file using stb_image_write
    bool success = RGBAImage::SaveImage(m_data, width, height, "../output/output.png");
    if (success) {
        std::cout << "Image saved successfully as output.png" << std::endl;
    } else {
        std::cerr << "Failed to save the image." << std::endl;
    }

    return 0;
}