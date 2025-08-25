#pragma once

#include "../../ECS/ECS.h"

#include <array>
#include <cassert>
#include <type_traits>

template<typename T, uint32_t Size>
class SortedCache
{
    static_assert(std::is_copy_assignable_v<T>, "T must be copyable");
    static_assert(std::is_copy_constructible_v<T>, "T must be copyable");
    static_assert(std::is_convertible_v<decltype(std::declval<const T&>() < std::declval<const T&>()), bool>, "T must have operator<");
    static_assert(std::is_convertible_v<decltype(std::declval<const T&>() == std::declval<const T&>()), bool>, "T must have operator==");

public:
    inline SortedCache() noexcept = default;

    ///Clears the cache completely
    inline constexpr void Initialize() noexcept
    {
        currentIndex = 0;
        count = 0;
    }

    ///Cache the impulse data of an entity pair. Function needs to be called with sorted entity keys (with entityA1 < entityB1 || (entityA1 == entityB1 && entityA2 < entityB2)).
    ///From entity with lsb to msb
    inline constexpr void Cache(const T& value)
    {
        assert(count < Size && "Trying to cache impulses, but buffer is full");
        assert((count == 0 || data[count - 1].EntityKey < value.EntityKey) && "Keys must be cached in sorted order");

        data[count++] = value;
    }

    ///Get impulses in same order as the entities have been cached. The entity key should also be in order.
    ///Current index needs to be 0 before calling this function for the first time
    inline constexpr bool TryGetImpulses(EntityPair entityPair, T& result) noexcept
    {
        while (currentIndex < count)
        {
            T& impulseData = data[currentIndex];
            if (impulseData.EntityKey.Key == entityPair.Key)
            {
                ++currentIndex;
                result = impulseData;
                return true;
            }

            if (entityPair.Key < impulseData.EntityKey.Key)
            {
                //Entity key does not exist in cache
                return false;
            }

            //Entity pair in list is not anymore present
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

private:
    uint32_t currentIndex;
    uint32_t count;
    std::array<T, Size> data;

    static_assert(std::is_trivially_default_constructible_v<T>, "Type for sorted cache needs to be trivial");
};