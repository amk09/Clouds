#include "rgba.h"


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb/stb_image_write.h"
#include <iostream>

bool RGBAImage::SaveImage(const std::vector<RGBA>& canvas, int width, int height, const std::string& filename) {
    
    if (canvas.size() != static_cast<size_t>(width * height)) {
        std::cerr << "Canvas size does not match the provided dimensions!" << std::endl;
        return false;
    }
    std::vector<uint8_t> image_data;
        for (const auto& pixel : canvas) {
            image_data.push_back(pixel.r);
            image_data.push_back(pixel.g);
            image_data.push_back(pixel.b);
            image_data.push_back(pixel.a); 
        }
    // Save image as PNG
    int result = stbi_write_png(filename.c_str(), width, height, 4, image_data.data(), width * 4);
    return result != 0;
    return true;
}
