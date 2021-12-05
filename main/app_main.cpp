#include "hardware/bluetooth/BluetoothManager.h"
#include "hardware/input/Button.h"
#include "hardware/input/InterruptButton.h"
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

    auto queue = button->queue();
    hardware::ButtonEvent number;
    for (;;)
    {
        if (queue == nullptr)
        {
            logging::Logger::error("MAIN", "Queue is null.");
            vTaskDelay(10);
            continue;
        }

        while (xQueueReceive(queue, &number, 10 / portTICK_PERIOD_MS))
        {
            logging::Logger::debug("MAIN", "Button event: {}", number == hardware::ButtonEvent::Pressed ? "Pressed" : "Release");
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
