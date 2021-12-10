#include "hardware/led/LedStrip.h"

#include "logging/Logger.h"

#include <algorithm>

std::string hardware::LedStrip::TAG = "LedStrip";

hardware::LedStrip::LedStrip(hardware::LedStripSettings settings)
{
    m_ledStrip.type = settings.ledStripType;
    m_ledStrip.is_rgbw = settings.hasWhite;
    m_ledStrip.length = settings.length;
    m_ledStrip.gpio = settings.gpio;
    m_ledStrip.brightness = 0;
    m_ledStrip.channel = settings.rmtChannel;
}

hardware::LedStrip::~LedStrip()
{
    led_strip_free(&m_ledStrip);
}

bool hardware::LedStrip::initialize()
{
    led_strip_install();

    auto initResult = led_strip_init(&m_ledStrip);

    if (initResult != ESP_OK)
    {
        logging::Logger::error(TAG, "Failed to initialize led strip.");
        return false;
    }

    return true;
}

esp_err_t hardware::LedStrip::update()
{
    return led_strip_flush(&m_ledStrip);
}

void hardware::LedStrip::setBrightness(uint8_t brightness)
{
    m_ledStrip.brightness = brightness;
}

esp_err_t hardware::LedStrip::setPixel(int pixel, Color color)
{
    return led_strip_set_pixel(&m_ledStrip, pixel, color.toRgb());
}

esp_err_t hardware::LedStrip::setPixels(int startPixel, int numPixels, Color color)
{
    return led_strip_fill(&m_ledStrip, startPixel, numPixels, color.toRgb());
}

esp_err_t hardware::LedStrip::setPixels(int startPixel, int numPixels, std::vector<Color> colors)
{
    std::vector<rgb_t> rgbColors;
    std::transform(colors.begin(), colors.end(), std::back_inserter(rgbColors), [](const Color& color) {
        return color.toRgb();
    });

    return led_strip_set_pixels(&m_ledStrip, startPixel, numPixels, rgbColors.data());
}

esp_err_t hardware::LedStrip::setAllPixels(Color color)
{
    return led_strip_fill(&m_ledStrip, 0, m_ledStrip.length, color.toRgb());
}
