#include "device/Device.h"
#include "logging/Logger.h"

#include "nvs_flash.h"

#include <memory>

extern "C"
{
    void app_main();
}

const static std::string TAG = "MAIN";

void app_main()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

    logging::Logger::setLogLevel(TAG, logging::LogLevel::Debug);

    auto device = std::make_unique<device::Device>();
    if (not device->initialize())
    {
        logging::Logger::error(TAG, "Could not initialize device.");
        esp_restart();
    }
}
