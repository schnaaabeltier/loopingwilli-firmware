#include "events/EventLoop.h"

#include "logging/Logger.h"

std::string events::EventLoop::TAG = "EventLoop";

events::EventLoop::~EventLoop()
{
    if (m_eventLoopHandle != nullptr)
    {
        esp_event_loop_delete(m_eventLoopHandle);
    }
}

bool events::EventLoop::initialize()
{
    esp_event_loop_args_t eventLoopArgs {};
    eventLoopArgs.queue_size = 10;
    eventLoopArgs.task_name = "eventLoopTask";
    eventLoopArgs.task_priority = uxTaskPriorityGet(nullptr);
    eventLoopArgs.task_stack_size = 2048;
    eventLoopArgs.task_core_id = tskNO_AFFINITY;

    auto creationResult = esp_event_loop_create(&eventLoopArgs, &m_eventLoopHandle);

    if (creationResult != ESP_OK)
    {
        logging::Logger::error(TAG, "Failed to create event loop: {}", esp_err_to_name(creationResult));
        return false;
    }


    bool registrationResult = registerEventHandler(m_eventLoopHandle, BUTTON_EVENTS, BUTTON_PRESSED_EVENT, EventLoop::onEventReceived, this, nullptr);
    registrationResult &= registerEventHandler(m_eventLoopHandle, BUTTON_EVENTS, BUTTON_RELEASED_EVENT, EventLoop::onEventReceived, this, nullptr);
    registrationResult &= registerEventHandler(m_eventLoopHandle, BLUETOOTH_EVENTS, BLUETOOTH_MESSAGE_RECEIVED_EVENT, EventLoop::onEventReceived, this, nullptr);

    if (not registrationResult)
    {
        logging::Logger::error(TAG, "Failed to register event handlers. Aborting...");
        return false;
    }

    return true;
}

bool events::EventLoop::registerEventHandler(esp_event_loop_handle_t eventLoop, esp_event_base_t eventBase,
                                             int32_t eventId, esp_event_handler_t eventHandler,
                                             void* eventHandlerArg, esp_event_handler_instance_t* instance)
{
    auto registrationResult = esp_event_handler_instance_register_with(eventLoop, eventBase,
                                                                       eventId, eventHandler, eventHandlerArg, instance);

    if (registrationResult != ESP_OK)
    {
        logging::Logger::error(TAG, "Failed to register event handler for {}-{}: {}", eventBase, eventId,
                               esp_err_to_name(registrationResult));
        return false;
    }

    return true;
}

void events::EventLoop::registerEventHandler(esp_event_base_t eventBase, int32_t eventId, const std::function<void(void*)>& handler)
{
    auto key = std::make_pair(eventBase, eventId);
    m_eventHandlers[key].push_back(handler);
}

void events::EventLoop::registerGenericEventHandler(const std::function<void(const Event&)>& handler)
{
    m_genericEventHandlers.push_back(handler);
}

void events::EventLoop::onEventReceived(void* handlerArgs, esp_event_base_t eventBase, int32_t eventId, void* eventData)
{
    auto eventLoop = static_cast<EventLoop*>(handlerArgs);
    eventLoop->handleEvent(eventBase, eventId, eventData);
}

void events::EventLoop::handleEvent(esp_event_base_t eventBase, int32_t eventId, void* eventDataPointer)
{
    auto key = std::make_pair(eventBase, eventId);
    auto handlers = m_eventHandlers[key];

    for (auto& handler : handlers)
    {
        handler(eventDataPointer);
    }

    for (auto& genericHandler : m_genericEventHandlers)
    {
        auto event = Event(eventBase, eventId, eventDataPointer);
        genericHandler(event);
    }
}

void events::EventLoop::postEvent(esp_event_base_t eventBase, int32_t eventId, void* eventData, std::size_t eventDataSize)
{
    esp_event_post_to(m_eventLoopHandle, eventBase, eventId, eventData, eventDataSize, 100 / portTICK_PERIOD_MS);
}

void events::EventLoop::postEventFromIsr(esp_event_base_t eventBase, int32_t eventId, void* eventData, std::size_t eventDataSize)
{
    auto result = esp_event_isr_post_to(m_eventLoopHandle, eventBase, eventId, eventData, eventDataSize, nullptr);
    if (result != ESP_OK)
    {
        ESP_DRAM_LOGE(TAG.c_str(), "Could not post to isr: %s", esp_err_to_name(result));
    }
}
