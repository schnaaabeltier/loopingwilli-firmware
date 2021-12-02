#include "hardware/bluetooth/BluetoothManager.h"
#include "hardware/input/Button.h"
#include "logging/Logger.h"

#include "nvs_flash.h"

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

    auto settings = hardware::BluetoothManagerSettings {};
    settings.useSecureSimplePairing = CONFIG_BT_SPP_ENABLED;
    auto manager = hardware::BluetoothManager(settings);
    manager.initialize();

//    auto button = hardware::Button(26, []() {
//        logging::Logger::debug("main", "Button 26 pressed.");
//    });
}
