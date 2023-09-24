#include "hardware/input/InterruptButton.h"
#include "logging/Logger.h"

#include "driver/gpio.h"

#include <chrono>
#include <cstdio>

std::string hardware::InterruptButton::TAG = "InterruptButton";

hardware::InterruptButton::InterruptButton(InterruptButtonConfig config, events::EventLoop* eventLoop) : m_gpio(config.gpioNumber),
    m_buttonMode(config.buttonMode), m_debounceMs(config.debounceMs), m_eventNumber(config.eventNumber), m_eventLoop(eventLoop)
{
    m_buttonState = isPressed() ? ButtonState::Pressed : ButtonState::Released;
}

void hardware::InterruptButton::globalGpioIsr(void* context)
{
    auto button = static_cast<InterruptButton*>(context);
    button->onButtonEventIsr();
}

void hardware::InterruptButton::onButtonEventIsr()
{
    auto currentTime = std::chrono::system_clock::now();

    // The button is currently debouncing and not yet finished
    if (debounceState() == DebounceState::Unstable and currentTime - m_lastDebounceStart < m_debounceMs)
        return;

    // Debouncing has finished
    if (debounceState() == DebounceState::Unstable and currentTime - m_lastDebounceStart >= m_debounceMs)
        setDebounceState(DebounceState::Debounced);

    bool isCurrentlyPressed = isPressed();
    ButtonState oldState = buttonState();
    bool stateChanged = (oldState == ButtonState::Pressed and not isCurrentlyPressed) or
            (oldState == ButtonState::Released and isCurrentlyPressed);

    // State has been changed, new debounce period
    if (stateChanged)
    {
        setDebounceState(DebounceState::Unstable);
        m_lastDebounceStart = std::chrono::system_clock::now();

        int32_t eventId = isCurrentlyPressed ? BUTTON_PRESSED_EVENT : BUTTON_RELEASED_EVENT;
        m_eventLoop->postEventFromIsr(BUTTON_EVENTS, eventId, static_cast<void*>(&m_eventNumber), sizeof(m_eventNumber));
    }

    setButtonState(isCurrentlyPressed ? ButtonState::Pressed : ButtonState::Released);
}

QueueHandle_t hardware::InterruptButton::queue()
{
    return m_queue;
}

bool hardware::InterruptButton::initialize()
{
    gpio_config_t gpioConfig {};
    gpioConfig.intr_type = GPIO_INTR_ANYEDGE;
    gpioConfig.mode = GPIO_MODE_INPUT;
    gpioConfig.pin_bit_mask = 1 << m_gpio;

    if (m_buttonMode == ButtonMode::ActiveLow)
    {
        gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
        gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
    }
    else
    {
        gpioConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
        gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    }

    auto configResult = gpio_config(&gpioConfig);

    if (configResult != ESP_OK)
    {
        logging::Logger::error(TAG, "Failed to configure GPIO {}: {}", (int) m_gpio, esp_err_to_name(configResult));
        return false;
    }

    m_queue = xQueueCreate(10, sizeof(uint32_t));

    if (m_queue == nullptr)
    {
        logging::Logger::error(TAG, "Failed to allocate memory for event queue.");
        return false;
    }

    auto installIsrResult = gpio_install_isr_service(0);
    if (installIsrResult != ESP_OK)
    {
        logging::Logger::error(TAG, "Failed to install ISR service: {}", esp_err_to_name(installIsrResult));
        return false;
    }

    auto addIsrHandlerResult = gpio_isr_handler_add(static_cast<gpio_num_t>(m_gpio), globalGpioIsr, this);
    if (addIsrHandlerResult != ESP_OK)
    {
        logging::Logger::error(TAG, "Failed to add ISR handler: {}", esp_err_to_name(addIsrHandlerResult));
        return false;
    }

    return true;
}

bool hardware::InterruptButton::isPressed() const
{
    int level = gpio_get_level(m_gpio);
    return m_buttonMode == ButtonMode::ActiveHigh ? level == 1 : level == 0;
}

hardware::DebounceState hardware::InterruptButton::debounceState() const
{
    return m_debounceState;
}

void hardware::InterruptButton::setDebounceState(DebounceState newState)
{
    std::lock_guard<std::mutex> lockGuard(m_debounceMutex);
    m_debounceState = newState;
}

hardware::ButtonState hardware::InterruptButton::buttonState() const
{
    return m_buttonState;
}

void hardware::InterruptButton::setButtonState(ButtonState newState)
{
    std::lock_guard<std::mutex> lockGuard(m_buttonMutex);
    m_buttonState = newState;
}
