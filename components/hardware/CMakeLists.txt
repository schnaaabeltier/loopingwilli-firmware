set(sources
        src/bluetooth/BluetoothManager.cpp
        src/input/Button.cpp
        src/input/InterruptButton.cpp)

idf_component_register(
    SRCS
        ${sources}
    INCLUDE_DIRS
        "include"
    REQUIRES
        bt
        button
        freertos
        logging
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)