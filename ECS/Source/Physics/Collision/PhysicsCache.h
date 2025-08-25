#pragma once

#include "ImpulseCache.h"

class PhysicsCache
{
public:
    inline PhysicsCache() noexcept = default;

    inline constexpr void Initialize() noexcept
    {
        impulseCache.Initialize();
    }

    ///Impulse cache

    ///Cache the impulse data of an entity pair. Function needs to be called with sorted entity keys (with entityA1 < entityB1 || (entityA1 == entityB1 && entityA2 < entityB2)).
    ///From entity with lsb to msb
    inline constexpr void CacheImpulseData(const ImpulseData& impulseData)
    {
        impulseCache.Cache(impulseData);
    }

    ///Get impulses in same order as the entities have been cached. The entity key should also be in order.
    ///Current index needs to be 0 before calling this function for the first time
    inline constexpr bool TryGetImpulseData(EntityPair entityPair, ImpulseData& outImpulsesData) noexcept
    {
        return impulseCache.TryGetImpulses(entityPair, outImpulsesData);
    }

    inline constexpr void ResetImpulses() noexcept
    {
        impulseCache.Reset();
    }

private:
    ImpulseCache impulseCache;
};