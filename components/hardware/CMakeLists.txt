set(sources
        src/bluetooth/BluetoothManager.cpp
        src/input/Button.cpp)

idf_component_register(
    SRCS
        ${sources}
    INCLUDE_DIRS
        "include"
    REQUIRES
        bt
        button
        logging
)
