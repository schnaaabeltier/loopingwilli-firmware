#include "events/EventData.h"

#include <cstring>

events::EventData::EventData(std::size_t oldSize, void* oldData) : size(oldSize), data(oldData)
{
    //data = std::malloc(oldSize);
    //std::memcpy(data, oldData, size);
}

events::EventData::EventData(const EventData& other)
{
    data = other.data;
    size = other.size;
    //data = std::malloc(other.size);
    //std::memcpy(data, other.data, other.size);
    //size = other.size;
}

events::EventData::~EventData()
{
    //std::free(data);
}
