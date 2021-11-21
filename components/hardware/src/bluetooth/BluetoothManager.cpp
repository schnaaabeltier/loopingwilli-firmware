#include "bluetooth/BluetoothManager.h"

#include "esp_bt.h"

bool hardware::BluetoothManager::initialize()
{
    bool hasError { false };

    esp_err_t memReleaseResult = esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
    hasError = (memReleaseResult != ESP_OK);
    if (memReleaseResult != ESP_OK)
    {

    }

    esp_bt_controller_config_t bluetoothConfig = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    auto initResult = esp_bt_controller_init(&bluetoothConfig);
    hasError = hasError or (initResult != ESP_OK);

    return hasError;
}
