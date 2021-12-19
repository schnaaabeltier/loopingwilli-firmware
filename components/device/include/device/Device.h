#pragma once

#include "hardware/bluetooth/BluetoothManager.h"
#include "hardware/input/InterruptButton.h"
#include "hardware/led/LedStrip.h"

#include <memory>
#include <vector>

namespace device
{
    class Device
    {
    public:
        Device();
        ~Device();

        bool initialize();

        events::EventLoop& eventLoop();

    private:
        static std::string TAG;

        std::unique_ptr<events::EventLoop> m_eventLoop { nullptr };
        std::unique_ptr<hardware::BluetoothManager> m_bluetoothManager { nullptr };
        std::vector<std::unique_ptr<hardware::InterruptButton>> m_buttons;
        std::vector<std::unique_ptr<hardware::LedStrip>> m_ledStrips;
    };
}
