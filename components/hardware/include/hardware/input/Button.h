#pragma once

#include "button/button.h"
#include "hardware/core/IInitializableHardware.h"

#include <functional>

namespace hardware
{
    class Button : public IIntializableHardware
    {
    public:
        Button(uint8_t gpio, std::function<void()> pressedCallback, button_active_level_t buttonActiveLevel = button_active_low);
        bool initialize() override;

    private:
        void onEvent(button_event_t event);
        static void globalButtonCallback(button_event_t event, void* context);

        std::function<void()> m_pressedCallback;
        uint8_t m_gpio { 0 };
        button_active_level_t m_buttonLevel { button_active_low };
    };
}
