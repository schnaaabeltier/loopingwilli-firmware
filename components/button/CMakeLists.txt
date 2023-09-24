idf_component_register(
    SRCS
        src/button/toggle.c
        src/button/port.c
        src/button/button.c
    INCLUDE_DIRS
        include
    REQUIRES
        driver
)
target_compile_options(${COMPONENT_LIB} PRIVATE -DESP_IDF)
