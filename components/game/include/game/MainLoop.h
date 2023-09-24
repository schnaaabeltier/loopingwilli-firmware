#pragma once

#include "device/Device.h"

#include "freertos/queue.h"

namespace game
{
    class MainLoop
    {
    public:
        MainLoop(device::Device* device);

        bool initialize();

        void run();

    private:
        void handlePendingEvents();
        void queueEvent(events::Event eventData);
        void setEventHandler(esp_event_base_t baseEvent, int32_t eventId, const std::function<void(void*)>& handler);

        device::Device* m_device { nullptr };

        QueueHandle_t m_eventQueue { nullptr };
        StaticQueue_t m_staticEventQueue {};
        uint8_t m_eventQueueBuffer[CONFIG_GAME_EVENT_QUEUE_SIZE * sizeof(events::Event)] {};

        std::map<std::pair<esp_event_base_t, int32_t>, std::function<void(void*)>> m_eventHandlers {};

        static std::string TAG;
    };
}