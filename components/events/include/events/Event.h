#pragma once

#include "EventData.h"

#include "esp_event.h"

namespace events
{
    class Event
    {
    public:
        Event() = default;
        Event(esp_event_base_t base, int32_t id, void* data);
        Event(const Event& other);

        esp_event_base_t eventBase;
        int32_t eventId;
        void* eventData;
    };
}
