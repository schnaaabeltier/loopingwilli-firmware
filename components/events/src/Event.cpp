#include "events/Event.h"

events::Event::Event(esp_event_base_t base, int32_t id, void* data) : eventBase(base), eventId(id), eventData(data)
{

}

events::Event::Event(const Event& other)
{
    eventBase = other.eventBase;
    eventId = other.eventId;
    eventData = other.eventData;
}
