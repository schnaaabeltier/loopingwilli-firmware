idf_component_register(
    SRCS
        src/button/toggle.cpp
        src/button/port.cpp
        src/button/button.cpp
    INCLUDE_DIRS
        include
)
target_compile_options(${COMPONENT_LIB} PRIVATE -DESP_IDF)
