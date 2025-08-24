#pragma once

#include "ECSSettings.h"

struct EntityPair
{
    EntityTwice Key;

    static constexpr EntityPair Make(Entity entity1, Entity entity2) noexcept
    {
        return EntityPair{ (static_cast<EntityTwice>(entity1) << 32) | entity2 };
    }

    inline constexpr bool operator<(const EntityPair& other) const noexcept
    {
        return Key < other.Key;
    }
};