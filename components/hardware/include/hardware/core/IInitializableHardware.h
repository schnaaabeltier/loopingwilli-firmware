#pragma once

namespace hardware
{
    class IIntializableHardware
    {
    public:
        virtual ~IIntializableHardware() {};

        virtual bool initialize() = 0;
    };
}