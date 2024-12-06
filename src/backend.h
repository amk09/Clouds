#ifndef BACKEND_H
#define BACKEND_H

#include <vector>
#include <iostream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "rgba.h"

bool saveImage(std::vector<RGBA> Image)
{
    std::vector<unsigned char> pixelData;
    for (const auto& pixel : Image) {
        pixelData.push_back(pixel.r);
        pixelData.push_back(pixel.g);
        pixelData.push_back(pixel.b);
        pixelData.push_back(pixel.a);
    }

    int result = stbi_write_png("output.png", 1000, 1000, 4, pixelData.data(), 1000 * 4);
    return result != 0;
}

#endif // BACKEND_H
