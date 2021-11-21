#include "hardware/bluetooth/BluetoothManager.h"

#include <utility>

#include "logging/Logger.h"

#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"

std::string hardware::BluetoothManager::TAG { "BluetoothManager" };
hardware::BluetoothManager* hardware::BluetoothManager::CALLBACK_INSTANCE { nullptr };

hardware::BluetoothManager::BluetoothManager(BluetoothManagerSettings settings) : m_settings(std::move(settings))
{
    setCallbackInstance(this);
}

void hardware::BluetoothManager::setCallbackInstance(BluetoothManager* manager)
{
    CALLBACK_INSTANCE = manager;
}

bool hardware::BluetoothManager::initialize()
{
    logging::Logger::info(TAG, "Starting to initialize.");

    esp_err_t memReleaseResult = esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
    if (memReleaseResult != ESP_OK)
    {
        logging::Logger::error(TAG, "Failed to release BLE memory: {}", esp_err_to_name(memReleaseResult));
        return false;
    }

    esp_bt_controller_config_t bluetoothConfig = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    bluetoothConfig.mode = ESP_BT_MODE_CLASSIC_BT;
    auto initResult = esp_bt_controller_init(&bluetoothConfig);
    if (initResult != ESP_OK)
    {
        logging::Logger::error(TAG, "Failed to initialize BT controller: {}", esp_err_to_name(initResult));
        return false;
    }

    auto enableResult = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
    if (enableResult != ESP_OK)
    {
        logging::Logger::error(TAG, "Failed to enable BT controller: {}", esp_err_to_name(enableResult));
        return false;
    }

    auto bdInitResult = esp_bluedroid_init();
    if (bdInitResult != ESP_OK)
    {
        logging::Logger::error(TAG, "Failed to initialize bluedroid: {}", esp_err_to_name(bdInitResult));
        return false;
    }

    auto bdEnableResult = esp_bluedroid_enable();
    if (bdEnableResult != ESP_OK)
    {
        logging::Logger::error(TAG, "Failed to enable bluedroid: {}", esp_err_to_name(bdEnableResult));
        return false;
    }

    auto gapCbResult = esp_bt_gap_register_callback(&BluetoothManager::gapCallback);
    if (gapCbResult != ESP_OK)
    {
        logging::Logger::error(TAG, "Failed to set BT gap callback: {}", esp_err_to_name(gapCbResult));
        return true;
    }

    auto sppCbResult = esp_spp_register_callback(&BluetoothManager::sppCallback);
    if (sppCbResult != ESP_OK)
    {
        logging::Logger::error(TAG, "Failed to set BT SPP callback: {}", esp_err_to_name(sppCbResult));
        return true;
    }

    auto sppInitResult = esp_spp_init(ESP_SPP_MODE_CB);
    if (sppInitResult != ESP_OK)
    {
        logging::Logger::error(TAG, "Failed to initialize BT SPP mode: {}", esp_err_to_name(sppInitResult));
        return false;
    }

    if (m_settings.useSecureSimplePairing)
    {
        esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
        esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
        auto setGapParamResult = esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
        if (setGapParamResult != ESP_OK)
        {
            logging::Logger::error(TAG, "Failed to set BT Gap security params: {}", esp_err_to_name(setGapParamResult));
            return false;
        }
    }
    else
    {
        esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
        esp_bt_pin_code_t pin_code;
        auto setPinResult = esp_bt_gap_set_pin(pin_type, 0, pin_code);
        if (setPinResult != ESP_OK)
        {
            logging::Logger::error(TAG, "Failed to set default pin for legacy BT pairing mode: {}", esp_err_to_name(setPinResult));
            return false;
        }
    }

    logging::Logger::info(TAG, "Successfully finished initialization.");

    return true;
}

void hardware::BluetoothManager::objGapCallback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* param)
{
    switch (event)
    {
        case ESP_BT_GAP_AUTH_CMPL_EVT:
        {
            if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS)
            {
                logging::Logger::info(TAG, "Successful GAP authentication of {}", param->auth_cmpl.device_name);
            }
            else
            {
                logging::Logger::error(TAG, "GAP authentication failed, status {}", param->auth_cmpl.stat);
            }
            break;
        }
        case ESP_BT_GAP_PIN_REQ_EVT:
        {
            logging::Logger::info(TAG, "Legacy pin pairing request (min 16 digits): {}", param->pin_req.min_16_digit);
            if (param->pin_req.min_16_digit)
            {
                logging::Logger::info(TAG, "Input pin code: 0000 0000 0000 0000");
                esp_bt_pin_code_t pin_code = { 0 };
                esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
            }
            else
            {
                logging::Logger::info(TAG, "Input pin code: 1234");
                esp_bt_pin_code_t pin_code;
                pin_code[0] = '1';
                pin_code[1] = '2';
                pin_code[2] = '3';
                pin_code[3] = '4';
                esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
            }
            break;
        }

        case ESP_BT_GAP_CFM_REQ_EVT:
        {
            logging::Logger::info(TAG, "SSP User confirmation request, please compare {}", param->cfm_req.num_val);
            esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
            break;
        }
        case ESP_BT_GAP_KEY_NOTIF_EVT:
        {
            logging::Logger::info(TAG, "SSP Passkey information: {}", param->key_notif.passkey);
            break;
        }
        case ESP_BT_GAP_KEY_REQ_EVT:
        {
            logging::Logger::info(TAG, "SSP Passkey request, please enter passkey!");
            break;
        }
        case ESP_BT_GAP_MODE_CHG_EVT:
        {
            logging::Logger::info(TAG, "Gap mode change event: {}", param->mode_chg.mode);
            break;
        }
        default:
        {
            logging::Logger::info(TAG, "Unspecified Gap event: {}", event);
            break;
        }
    }
}

void hardware::BluetoothManager::objSppCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t* param)
{
    switch (event)
    {
        case ESP_SPP_INIT_EVT:
        {
            logging::Logger::info(TAG, "SPP init event");
            esp_spp_start_srv(ESP_SPP_SEC_AUTHENTICATE,ESP_SPP_ROLE_SLAVE, 0, m_settings.sppServerName.c_str());
            break;
        }
        case ESP_SPP_DISCOVERY_COMP_EVT:
        {
            logging::Logger::info(TAG, "SPP discovery completed event");
            break;
        }
        case ESP_SPP_OPEN_EVT:
        {
            logging::Logger::info(TAG, "SPP open event");
            break;
        }
        case ESP_SPP_CLOSE_EVT:
        {
            logging::Logger::info(TAG, "SPP close event");
            break;
        }
        case ESP_SPP_START_EVT:
        {
            logging::Logger::info(TAG, "SPP start event");
            esp_bt_dev_set_device_name(m_settings.sppDeviceName.c_str());
            esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
            break;
        }
        case ESP_SPP_CL_INIT_EVT:
        {
            logging::Logger::info(TAG, "SPP cl init event");
            break;
        }
        case ESP_SPP_DATA_IND_EVT:
        {
            logging::Logger::info(TAG, "SPP data event from handle {}", param->data_ind.handle);
            const char* data = reinterpret_cast<const char*>(param->data_ind.data);
            std::size_t length = param->data_ind.len;
            auto string = std::string(data, length);
            logging::Logger::info(TAG, "Data is {}", string);
            break;
        }
        case ESP_SPP_CONG_EVT:
        {
            logging::Logger::info(TAG, "SPP CONG event");
            break;
        }
        case ESP_SPP_WRITE_EVT:
        {
            logging::Logger::info(TAG, "SPP write event");
            break;
        }
        case ESP_SPP_SRV_OPEN_EVT:
        {
            logging::Logger::info(TAG, "SPP server open event");
            break;
        }
        case ESP_SPP_SRV_STOP_EVT:
        {
            logging::Logger::info(TAG, "SPP server stop event");
            break;
        }
        case ESP_SPP_UNINIT_EVT:
        {
            logging::Logger::info(TAG, "SPP uninit event");
            break;
        }
        default:
        {
            logging::Logger::info(TAG, "Unspecified SPP event");
            break;
        }
    }
}

void hardware::BluetoothManager::gapCallback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* param)
{
    auto manager = BluetoothManager::CALLBACK_INSTANCE;
    if (manager != nullptr)
    {
        manager->objGapCallback(event, param);
    }
}

void hardware::BluetoothManager::sppCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t* param)
{
    auto manager = BluetoothManager::CALLBACK_INSTANCE;
    if (manager != nullptr)
    {
        manager->objSppCallback(event, param);
    }
}
