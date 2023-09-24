set(sources
        include/logging/Logger.h
        src/Logger.cpp)

idf_component_register(
        SRCS
            ${sources}
        INCLUDE_DIRS
            "include"
        REQUIRES
            log
            fmt
)
