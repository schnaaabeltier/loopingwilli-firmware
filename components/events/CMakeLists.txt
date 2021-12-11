set(sources
        src/Events.cpp
        src/EventLoop.cpp)

idf_component_register(
    SRCS
        ${sources}
    INCLUDE_DIRS
        "include"
    REQUIRES
        logging)

