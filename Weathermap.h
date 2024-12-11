#ifndef WEATHERMAP_H
#define WEATHERMAP_H

#include <vector>
#include "glm/glm.hpp"
#include <stdexcept>

class WeatherMap {
public:
    // Constructor to initialize the weather map with the given dimensions
    WeatherMap(int width=512, int height=512)
        : width(width), height(height),
          redChannel(width * height, 0.0f), //used to specify where cloudscan appear
          greenChannel(width * height, 0.0f), //used to specify where cloudscan appear
          blueChannel(width * height, 0.0f), //The blue color channel describes the maximum cloud height (in 0,1)
          alphaChannel(width * height, 0.0f) {} //the alpha channel describes the cloud density (in 0,1)

    // Set a pixel value for a specific channel
    void setPixel(int x, int y, char channel, float value) {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            throw std::out_of_range("Pixel coordinates are out of range.");
        }

        switch (channel) {
            case 'r':
                redChannel[y * width + x] = value;
                break;
            case 'g':
                greenChannel[y * width + x] = value;
                break;
            case 'b':
                blueChannel[y * width + x] = value;
                break;
            case 'a':
                alphaChannel[y * width + x] = value;
                break;
            default:
                throw std::invalid_argument("Invalid channel. Use 'r', 'g', 'b', or 'a'.");
        }
    }

    // Get a pixel value from a specific channel
    float getPixel(int x, int y, char channel) const {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            throw std::out_of_range("Pixel coordinates are out of range.");
        }

        switch (channel) {
            case 'r':
                return redChannel[y * width + x];
            case 'g':
                return greenChannel[y * width + x];
            case 'b':
                return blueChannel[y * width + x];
            case 'a':
                return alphaChannel[y * width + x];
            default:
                throw std::invalid_argument("Invalid channel. Use 'r', 'g', 'b', or 'a'.");
        }
    }

    // Sample the weather map at normalized UV coordinates for a specific channel
    float sample(float u, float v, char channel) const {
        // Clamp UV to [0, 1]
        u = glm::clamp(u, 0.0f, 1.0f);
        v = glm::clamp(v, 0.0f, 1.0f);

        // Convert UV to pixel space
        int x = static_cast<int>(u * (width - 1));
        int y = static_cast<int>(v * (height - 1));

        return getPixel(x, y, channel);
    }

    // Getters for dimensions
    int getWidth() const { return width; }
    int getHeight() const { return height; }



    

private:
    int width;  // Width of the weather map
    int height; // Height of the weather map

    // Separate 2D textures for each channel
    std::vector<float> redChannel;
    std::vector<float> greenChannel;
    std::vector<float> blueChannel;
    std::vector<float> alphaChannel;
};

#endif // WEATHERMAP_H
