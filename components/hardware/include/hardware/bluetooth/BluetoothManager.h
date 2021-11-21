#pragma once

#include "hardware/core/IInitializableHardware.h"

#include "esp_gap_bt_api.h"
#include "esp_spp_api.h"

#include <string>

namespace hardware
{
    struct BluetoothManagerSettings
    {
        bool useSecureSimplePairing { true };
        std::string sppServerName { "SPPServer" };
        std::string sppDeviceName { "Looping Willi" };
    };

    class BluetoothManager : public IIntializableHardware
    {
    public:
        explicit BluetoothManager(BluetoothManagerSettings settings);

        bool initialize() override;
        static void setCallbackInstance(BluetoothManager* manager);

        static void sppCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t* param);
        static void gapCallback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* param);

        void objSppCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t* param);
        void objGapCallback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* param);

        static std::string TAG;
        static BluetoothManager* CALLBACK_INSTANCE;

    private:
        BluetoothManagerSettings m_settings;
    };
}