#include "events/EventLoop.h"
#include "hardware/bluetooth/BluetoothManager.h"
#include "hardware/input/Button.h"
#include "hardware/input/InterruptButton.h"
#include "hardware/led/LedStrip.h"
#include "logging/Logger.h"

#include "nvs_flash.h"

#include <memory>

extern "C"
{
    void app_main();
}

void app_main()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    logging::Logger::setLogLevel("MAIN", logging::LogLevel::Debug);

    auto settings = hardware::BluetoothManagerSettings {};
    settings.useSecureSimplePairing = CONFIG_BT_SPP_ENABLED;
    auto manager = hardware::BluetoothManager(settings);
    manager.initialize();

    hardware::InterruptButtonConfig config {};
    config.gpioNumber = GPIO_NUM_26;
    config.buttonMode = hardware::ButtonMode::ActiveLow;
    config.debounceMs = std::chrono::milliseconds(50);

    auto button = new hardware::InterruptButton(config);
    button->initialize();

    hardware::LedStripSettings ledSettings {};
    ledSettings.length = 3;
    ledSettings.gpio = GPIO_NUM_19;
    auto ledStrip = hardware::LedStrip(ledSettings);
    ledStrip.initialize();
    ledStrip.setBrightness(255);
    ledStrip.setAllPixels(hardware::Color(125, 125, 125));
    ledStrip.update();

    auto eventLoop = events::EventLoop();
    eventLoop.initialize();
    eventLoop.registerEventHandler(BUTTON_EVENTS, BUTTON_PRESSED_EVENT, [](void*) {
        //logging::Logger::info("MAIN", "Event handler called.");
    });

    for (int i = 0; i < 255; i++)
    {
        auto color = hardware::Color(i, i, i);
        ledStrip.setPixel(0, hardware::Color(i, 0, 0));
        ledStrip.setPixel(1, hardware::Color(0, i, 0));
        ledStrip.setPixel(2, hardware::Color(0, 0, i));
        ledStrip.update();
        eventLoop.postEvent(BUTTON_EVENTS, BUTTON_PRESSED_EVENT, nullptr);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
