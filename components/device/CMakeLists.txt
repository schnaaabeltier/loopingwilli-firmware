set(sources
        src/Device.cpp)

idf_component_register(
        SRCS
            ${sources}
        INCLUDE_DIRS
            "include"
        REQUIRES
            hardware
            logging)
