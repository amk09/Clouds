#ifndef RGBA_H
#define RGBA_H

#pragma once

#include <cstdint>

struct RGBA {
    unsigned char r, g, b, a;

    RGBA(unsigned char red = 0, unsigned char green = 0, unsigned char blue = 0, unsigned char alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}
};
#endif // RGBA_H
