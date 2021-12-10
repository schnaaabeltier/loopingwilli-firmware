#pragma once

extern "C"
{
    #include "rgb.h"
}

namespace hardware
{
    struct Color
    {
        uint8_t r { 0 };
        uint8_t g { 0 };
        uint8_t b { 0 };

        Color(uint8_t r, uint8_t g, uint8_t b);

        rgb_t toRgb() const;
    };
}