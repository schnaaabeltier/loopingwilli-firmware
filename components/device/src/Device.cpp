#include "device/Device.h"

#include "logging/Logger.h"

std::string device::Device::TAG = "Device";

device::Device::Device()
{
}

device::Device::~Device()
{
}

bool device::Device::initialize()
{
    m_eventLoop = std::make_unique<events::EventLoop>();
    bool initSuccess = m_eventLoop->initialize();

    auto bluetoothSettings = hardware::BluetoothManagerSettings {};
    bluetoothSettings.sppDeviceName = "Looping Willi";
    m_bluetoothManager = std::make_unique<hardware::BluetoothManager>(bluetoothSettings);

    auto buttonSettings = hardware::InterruptButtonConfig {};
    buttonSettings.gpioNumber = GPIO_NUM_26;
    buttonSettings.buttonMode = hardware::ButtonMode::ActiveLow;
    m_buttons.push_back(std::make_unique<hardware::InterruptButton>(buttonSettings, m_eventLoop.get()));

    auto ledSettings = hardware::LedStripSettings {};
    ledSettings.gpio = GPIO_NUM_19;
    ledSettings.length = 3;
    m_ledStrips.push_back(std::make_unique<hardware::LedStrip>(ledSettings));

    initSuccess &= m_bluetoothManager->initialize();

    for (auto& button : m_buttons)
    {
        initSuccess &= button->initialize();
    }

    for (auto& led : m_ledStrips)
    {
        initSuccess &= led->initialize();
    }

    if (not initSuccess)
    {
        logging::Logger::error(TAG, "Could not initialize hardware.");
        return false;
    }

    return true;
}

events::EventLoop& device::Device::eventLoop()
{
    return *m_eventLoop;
}