#pragma once

#include "Events.h"

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace events
{
    class EventLoop
    {
    public:
        bool initialize();

        void registerEventHandler(esp_event_base_t eventBase, int32_t eventId, const std::function<void(void*)>& handler);
        void postEvent(esp_event_base_t eventBase, int32_t eventId, void* eventData);

    private:
        static std::string TAG;

        static void onEventReceived(void* handlerArgs, esp_event_base_t eventBase, int32_t eventId, void* eventData);
        void handleEvent(esp_event_base_t eventBase, int32_t eventId, void* eventData);

        static bool registerEventHandler(esp_event_loop_handle_t event_loop, esp_event_base_t event_base, int32_t event_id,
                                  esp_event_handler_t event_handler, void *event_handler_arg, esp_event_handler_instance_t *instance);

        esp_event_loop_handle_t m_eventLoopHandle { nullptr };

        std::map<std::pair<esp_event_base_t, int32_t>, std::vector<std::function<void(void*)>>> m_eventHandlers;
    };
}