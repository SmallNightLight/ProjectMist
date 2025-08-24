#pragma once

#include "../../ECS/ECS.h"
#include "ContactPair.h"

#include <array>
#include <cassert>
#include <type_traits>

class ImpulseCache
{
public:
    inline ImpulseCache() noexcept = default;

    ///Clears the cache completely
    inline constexpr void Initialize() noexcept
    {
        currentIndex = 0;
        count = 0;
    }

    ///Cache the impulse data of an entity pair. Function needs to be called with sorted entity keys (with entityA1 < entityB1 || (entityA1 == entityB1 && entityA2 < entityB2)).
    ///From entity with lsb to msb
    inline constexpr void Cache(const ImpulseData& impulseData)
    {
        assert(count < MaxSize && "Trying to cache impulses, but buffer is full");
        assert((count == 0 || data[count - 1].EntityKey < impulseData.EntityKey) && "Keys must be cached in sorted order");

        data[count++] = impulseData;
    }

    ///Get impulses in same order as the entities have been cached. The entity key should also be in order.
    ///Current index needs to be 0 before calling this function for the first time
    inline constexpr bool TryGetImpulses(EntityPair entityPair, ImpulseData& outImpulsesData) noexcept
    {
        while (currentIndex < count)
        {
            ImpulseData& impulseData = data[currentIndex];
            if (impulseData.EntityKey.Key == entityPair.Key)
            {
                ++currentIndex;
                outImpulsesData = impulseData;
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
    static constexpr uint32_t MaxSize = MAXENTITIES;

    uint32_t currentIndex;
    uint32_t count;
    std::array<ImpulseData, MaxSize> data;
};

static_assert(std::is_trivially_default_constructible_v<ImpulseCache>, "Impulse cache needs to be trivial");