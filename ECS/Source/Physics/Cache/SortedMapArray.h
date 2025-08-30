#pragma once

#include "SortedMap.h"

#include <array>
#include <cstdint>

template<typename KeyType, typename ValueType, uint32_t iterationCount, uint32_t size>
class SortedMapArray
{
public:
    inline SortedMapArray() noexcept = default;

    ///Clears the cache completely
    inline constexpr void Initialize() noexcept
    {
        Reset();
    }

    inline constexpr void NextIteration()
    {
        assert(currentIteration < iterationCount - 1 && "Exceeded iteration count");
        ++currentIteration;
    }

    inline constexpr void Cache(const ValueType& value)
    {
        data[currentIteration].Cache(value);
    }

    inline constexpr bool TryGet(KeyType key, ValueType& result) noexcept
    {
        return data[currentIteration].TryGet(key, result);
    }

    ///Clears the cache completely
    inline constexpr void Reset() noexcept
    {
        currentIteration = 0;
    }

private:
    uint32_t currentIteration;
    std::array<SortedMap<KeyType, ValueType, size>, iterationCount> data;
};