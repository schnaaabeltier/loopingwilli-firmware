#pragma once

#include "core/IInitializableHardware.h"

namespace hardware
{
    class BluetoothManager : public IIntializableHardware
    {
    public:
        bool initialize() override;
    };
}