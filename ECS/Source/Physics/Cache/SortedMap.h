#pragma once

#include "../../ECS/ECS.h"

#include <array>
#include <cassert>
#include <type_traits>

template<typename KeyType, typename ValueType, uint32_t Size>
class SortedMap
{
    static_assert(std::is_copy_assignable_v<ValueType>, "Value type must be copy assignable");
    static_assert(std::is_copy_constructible_v<ValueType>, "Value type must be copy constructible");
    static_assert(std::is_convertible_v<decltype(std::declval<const ValueType&>() < std::declval<const ValueType&>()), bool>, "ValueType must support operator< with ValueType");
    static_assert(std::is_convertible_v<decltype(std::declval<const ValueType&>() == std::declval<const KeyType&>()), bool>, "ValueType must support operator== with KeyType");
    static_assert(std::is_convertible_v<decltype(std::declval<const KeyType&>() < std::declval<const ValueType&>()), bool>, "KeyType must support operator< with ValueType");

public:
    inline SortedMap() noexcept = default;

    ///Clears the cache completely
    inline constexpr void Initialize() noexcept
    {
        Reset();
    }

    ///Cache the value. Function needs to be called with sorted keys (with keyA1 < keyB1 || (keyA1 == keyB1 && keyA2 < keyB2)).
    ///From key with lsb to msb
    inline constexpr void Cache(const ValueType& value)
    {
        assert(count < Size && "Trying to cache value, but buffer is full");
        assert((count == 0 || data[count - 1] < value) && "Keys must be cached in sorted order");

        data[count++] = value;
    }

    ///Get value in same order as the keys have been cached. The key should also be in order.
    ///Current index needs to be 0 before calling this function for the first time
    inline constexpr bool TryGet(KeyType key, ValueType& result) noexcept
    {
        while (currentIndex < count)
        {
            ValueType& currentValue = data[currentIndex];

            if (key < currentValue)
            {
                //key does not exist in cache
                return false;
            }

            if (currentValue == key)
            {
                ++currentIndex;
                result = currentValue;
                return true;
            }

            //Key in list is not anymore present
            ++currentIndex;
        }

        return false;
    }

    inline constexpr bool HasKey(KeyType key) noexcept
    {
        while (currentIndex < count)
        {
            ValueType& currentValue = data[currentIndex];

            if (key < currentValue)
            {
                //key does not exist in cache
                return false;
            }

            if (currentValue == key)
            {
                ++currentIndex;
                return true;
            }

            //Key in list is not anymore present
            ++currentIndex;
        }

        return false;
    }

    ///Sets the internal buffer index to 0, but keeps all data in the cache
    inline constexpr void Reset() noexcept
    {
        currentIndex = 0;
        count = 0;
    }

    inline constexpr void ResetIndex() noexcept
    {
        currentIndex = 0;
    }

private:
    uint32_t currentIndex;
    uint32_t count;
    std::array<ValueType, Size> data;

    static_assert(std::is_trivially_default_constructible_v<ValueType>, "Type for sorted cache needs to be trivial");
};