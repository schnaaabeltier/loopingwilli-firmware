#pragma once

#include <cstdlib>

namespace events
{
    class EventData
    {
    public:
        EventData() = default;
        EventData(std::size_t size, void* data);
        EventData(const EventData& other);
        ~EventData();

        std::size_t size { 0 };
        void* data { nullptr };
    };
}