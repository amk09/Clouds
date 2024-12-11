#ifndef NOISEMAP_H
#define NOISEMAP_H

#include <vector>
#include "glm/glm.hpp"
#include <stdexcept>
#include <iostream>

class NoiseMap {
public:
    // Constructor to initialize the weather map with the given dimensions
    NoiseMap(int length = 128, int height = 128, int depth = 128)
        : width(length), height(height), depth(depth),
           redChannel(length * height * depth, 0.0f), //Low frequency Perlin-Worley;    red and the following all in [0,1]
          greenChannel(length * height * depth, 0.0f), //Medium frequency Worley noise
          blueChannel(length * height * depth, 0.0f), //High frequency Worley noise
           alphaChannel(length * height * depth, 0.0f) {} //the alpha channel Highest frequency Worley noise

    // Set a pixel value for a specific channel
    void setPixel(int x, int y, int z, char channel, float value) {
        if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth) {
            throw std::out_of_range("Voxel coordinates are out of range.");
        }

        int index = (z * height * width) + (y * width) + x;

        switch (channel) {
            case 'r':
                redChannel[index] = value;
                break;
            case 'g':
                greenChannel[index] = value;
                break;
            case 'b':
                blueChannel[index] = value;
                break;
            case 'a':
                alphaChannel[index] = value;
                break;
            default:
                throw std::invalid_argument("Invalid channel. Use 'r', 'g', 'b', or 'a'.");
        }
    }

    // Get a pixel value from a specific channel
    float getPixel(int x, int y, int z, char channel) const {
        if (x < 0 || x >= width || y < 0 || y >= height || z < 0 || z >= depth) {
            throw std::out_of_range("Voxel coordinates are out of range.");
        }

        int index = (z * height * width) + (y * width) + x;

        switch (channel) {
            case 'r':
                return redChannel[index];
            case 'g':
                return greenChannel[index];
            case 'b':
                return blueChannel[index];
            case 'a':
                return alphaChannel[index];
            default:
                throw std::invalid_argument("Invalid channel. Use 'r', 'g', 'b', or 'a'.");
        }
    }

    // Sample the weather map at normalized UV coordinates for a specific channel
     float sample(float u, float v, float w, char channel) const {
        // Clamp UVW to [0, 1]
        u = glm::clamp(u, 0.0f, 1.0f);
        v = glm::clamp(v, 0.0f, 1.0f);
        w = glm::clamp(w, 0.0f, 1.0f);

        // Convert UVW to voxel space
        int x = static_cast<int>(u * (width - 1));
        int y = static_cast<int>(v * (height - 1));
        int z = static_cast<int>(w * (depth - 1));

        return getPixel(x, y, z, channel);
    }

    // Getters for dimensions
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getDepth() const { return depth; }


    

private:
    int width;  // Width of the noise map
    int height; // Height of the noise map
    int depth;
    // Separate 2D textures for each channel
    std::vector<float> redChannel;
    std::vector<float> greenChannel;
    std::vector<float> blueChannel;
    std::vector<float> alphaChannel;
};

#endif // NOISEMAP_H