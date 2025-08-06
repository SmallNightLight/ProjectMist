#pragma once

#include <array>
#include <cassert>

#include "ECSSettings.h"

template<uint32_t Capacity>
class EntityQueue //TODO: should not be sparse set but trivial set?
{
public:
    EntityQueue() = default;

    void Push(Entity value)
    {
        assert(size < Capacity && "EntityQueue overflow");
        data[tail] = value;
        tail = (tail + 1) % Capacity;
        ++size;
    }

    Entity Front() const
    {
        assert(size > 0 && "EntityQueue is empty");
        return data[head];
    }

    void Pop()
    {
        assert(size > 0 && "EntityQueue is empty");
        head = (head + 1) % Capacity;
        --size;
    }

    inline bool Empty() const
    {
        return size == 0;
    }

    inline std::size_t Count() const
    {
        return size;
    }

private:
    std::array<Entity, Capacity> data { };
    uint32_t head = 0;
    uint32_t tail = 0;
    uint32_t size = 0;
};