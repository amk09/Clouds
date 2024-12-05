#pragma once

#include <cstdint>
#include <vector>
#include <string>

struct RGBA {
    std::uint8_t r;
    std::uint8_t g;
    std::uint8_t b;
    std::uint8_t a = 255;
    
    public:
    int m_width = 0;
    int m_height = 0;


    
};

struct RGBAf {

    float r;
    float g;
    float b;
    float a = 255;


    RGBA toRGBA() {
        RGBA result;


        result.r = static_cast<uint8_t>(std::min(255.0f,std::max(0.0f, r)));
        result.g = static_cast<uint8_t>(std::min(255.0f,std::max(0.0f, g)));
        result.b = static_cast<uint8_t>(std::min(255.0f,std::max(0.0f, b)));
        result.a = static_cast<uint8_t>(std::min(255.0f,std::max(0.0f, a)));
        return result;
    }

    static RGBAf fromRGBA(RGBA pixel) {
        
        return RGBAf{ float(pixel.r), float(pixel.g), float(pixel.b), float(pixel.a) };
    }

};

class RGBAImage {
public:
    static bool SaveImage(const std::vector<RGBA>& canvas, int width, int height, const std::string& filename);
};