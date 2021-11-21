set(sources
        src/bluetooth/BluetoothManager.cpp)

idf_component_register(
    SRCS
        ${sources}
    INCLUDE_DIRS
        "include"
    REQUIRES
        bt
        logging
)
