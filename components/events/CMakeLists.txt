set(sources
        src/Event.cpp
        src/EventData.cpp
        src/Events.cpp
        src/EventLoop.cpp)

idf_component_register(
    SRCS
        ${sources}
    INCLUDE_DIRS
        "include"
    REQUIRES
        esp_event
        logging)

