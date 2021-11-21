#pragma once

namespace hardware
{
    class IIntializableHardware
    {
    public:
        virtual bool initialize() = 0;
    };
}