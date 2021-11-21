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
)

add_subdirectory(lib/fmt)

target_link_libraries(__idf_logging PUBLIC fmt)