#include "hardware/led/Color.h"

hardware::Color::Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b)
{

}

rgb_t hardware::Color::toRgb() const
{
    return rgb_from_values(r, g, b);
}
