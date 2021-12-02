#include "hardware/input/Button.h"

extern "C"
{
    #include "button/button.h"
}

#include <utility>

hardware::Button::Button(uint8_t gpio, std::function<void()> pressedCallback, button_active_level_t buttonActiveLevel)
    : m_pressedCallback(std::move(pressedCallback)), m_gpio(gpio), m_buttonLevel(buttonActiveLevel)
{
}

bool hardware::Button::initialize()
{
    button_config_t config = BUTTON_CONFIG(m_buttonLevel);
    int success = button_create(m_gpio, config, &Button::globalButtonCallback, this);
    return success != 0;
}

void hardware::Button::onEvent(button_event_t event)
{
    switch (event)
    {
        case button_event_single_press:
            if (m_pressedCallback)
                m_pressedCallback();
            break;
        case button_event_double_press:
        case button_event_tripple_press:
        case button_event_long_press:
            break;
    }
}

void hardware::Button::globalButtonCallback(button_event_t event, void* context)
{
    auto* button = static_cast<Button*>(context);
    if (button != nullptr)
    {
        button->onEvent(event);
    }
}
