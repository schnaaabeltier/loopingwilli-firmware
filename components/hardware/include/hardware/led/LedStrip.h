#pragma once

#include "hardware/core/IInitializableHardware.h"
#include "hardware/led/Color.h"

extern "C"
{
    #include "led_strip.h"
}

#include <string>
#include <vector>

namespace hardware
{
    struct LedStripSettings
    {
        led_strip_type_t ledStripType { LED_STRIP_SK6812 };
        int length { 4 };
        bool hasWhite { true };
        gpio_num_t gpio { GPIO_NUM_0 };
        rmt_channel_t rmtChannel { RMT_CHANNEL_0 };
    };

    class LedStrip : public IIntializableHardware
    {
    public:
        explicit LedStrip(LedStripSettings settings);
        ~LedStrip();

        bool initialize() override;

        void setBrightness(uint8_t brightness);
        esp_err_t setPixel(int pixel, Color color);
        esp_err_t setPixels(int startPixel, int numPixels, Color color);
        esp_err_t setPixels(int startPixel, int numPixels, std::vector<Color> colors);
        esp_err_t setAllPixels(Color color);

        esp_err_t update();

    private:
        static std::string TAG;

        led_strip_t m_ledStrip {};
    };
}