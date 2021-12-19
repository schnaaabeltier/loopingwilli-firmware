#include "game/MainLoop.h"

#include "logging/Logger.h"

std::string game::MainLoop::TAG = "MainLoop";

game::MainLoop::MainLoop(device::Device* device) : m_device(device)
{

}

bool game::MainLoop::initialize()
{
    m_eventQueue = xQueueCreateStatic(CONFIG_GAME_EVENT_QUEUE_SIZE, sizeof(events::Event), m_eventQueueBuffer, &m_staticEventQueue);

    if (m_eventQueue == nullptr)
    {
        logging::Logger::error(TAG, "Could not create game event queue.");
        return false;
    }

    setEventHandler(BUTTON_EVENTS, BUTTON_PRESSED_EVENT, [this](void* eventData) {
        int* buttonId = static_cast<int*>(eventData);
        logging::Logger::info(TAG, "Button {} was pressed", *buttonId);
    });

    m_device->eventLoop().registerGenericEventHandler([this](const events::Event& event) {
        queueEvent(event);
    });

    return true;
}

void game::MainLoop::queueEvent(events::Event event)
{
    xQueueSend(m_eventQueue, &event, 100 / portTICK_PERIOD_MS);
}

void game::MainLoop::setEventHandler(esp_event_base_t baseEvent, int32_t eventId, const std::function<void(void*)>& handler)
{
    auto key = std::make_pair(baseEvent, eventId);
    m_eventHandlers[key] = handler;
}

void game::MainLoop::handlePendingEvents()
{
    std::size_t eventCount { 0 };
    events::Event event {};
    while (xQueueReceive(m_eventQueue, &event, 10 / portTICK_PERIOD_MS) and eventCount < CONFIG_GAME_EVENT_QUEUE_SIZE)
    {
        logging::Logger::info(TAG, "Read game event with id {}-{}", event.eventBase, event.eventId);
        logging::Logger::info(TAG, "Free heap: {} Bytes", esp_get_free_heap_size());

        auto key = std::make_pair(event.eventBase, event.eventId);
        if (m_eventHandlers.count(key) > 0)
        {
            m_eventHandlers[key](event.eventData);
        }
    }
}

void game::MainLoop::run()
{
    while (true)
    {
        handlePendingEvents();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
