#pragma once

#include "hardware/core/IInitializableHardware.h"

#include "driver/gpio.h"
#include "esp_intr_alloc.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <mutex>
#include <string>

namespace hardware
{
    enum class ButtonMode
    {
        ActiveHigh = 0,
        ActiveLow = 1
    };

    enum class DebounceState
    {
        Debounced,
        Unstable
    };

    enum class ButtonState
    {
        Pressed,
        Released
    };

    enum class ButtonEvent
    {
        Pressed,
        Released
    };

    struct InterruptButtonConfig
    {
        gpio_num_t gpioNumber { GPIO_NUM_0 };
        std::chrono::milliseconds debounceMs { 50 };
        ButtonMode buttonMode { ButtonMode::ActiveHigh };
    };

    class InterruptButton : public IIntializableHardware
    {
    public:
        explicit InterruptButton(InterruptButtonConfig config);
        bool initialize() override;

        xQueueHandle queue();
    private:
        void IRAM_ATTR onButtonEventIsr();
        static void IRAM_ATTR globalGpioIsr(void* context);
        bool isPressed() const;

        DebounceState debounceState() const;
        void setDebounceState(DebounceState newState);

        ButtonState buttonState() const;
        void setButtonState(ButtonState newState);

        gpio_num_t m_gpio { GPIO_NUM_0 };
        ButtonMode m_buttonMode { ButtonMode::ActiveLow };
        std::chrono::milliseconds m_debounceMs { 50 };
        DebounceState m_debounceState { DebounceState::Debounced };
        ButtonState m_buttonState { ButtonState::Released };
        xQueueHandle m_queue { nullptr };
        std::chrono::system_clock::time_point m_lastDebounceStart;

        std::mutex m_debounceMutex {};
        std::mutex m_buttonMutex {};

        static std::string TAG;
    };
}