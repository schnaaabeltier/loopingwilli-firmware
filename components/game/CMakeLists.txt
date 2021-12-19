set(sources
    src/MainLoop.cpp
)

idf_component_register(
    SRCS
        ${sources}
    INCLUDE_DIRS
        "include"
    REQUIRES
        device)
