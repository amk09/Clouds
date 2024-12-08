#ifndef BACKEND_H
#define BACKEND_H

#include <vector>
#include <iostream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "rgba.h"

bool saveImage(const std::vector<RGBA>& Image, const std::string& filename, int width = 1000, int height = 1000) {
    std::vector<unsigned char> pixelData;
    for (const auto& pixel : Image) {
        pixelData.push_back(pixel.r);
        pixelData.push_back(pixel.g);
        pixelData.push_back(pixel.b);
        pixelData.push_back(pixel.a);
    }

    // Use stb_image_write to save the image as PNG
    int result = stbi_write_png(filename.c_str(), width, height, 4, pixelData.data(), width * 4);
    
    return result != 0; // Return true if saving was successful
}

#endif // BACKEND_H
