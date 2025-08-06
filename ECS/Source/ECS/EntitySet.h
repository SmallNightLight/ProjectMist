#pragma once

#include "ECSSettings.h"

#include <cstdint>

template<uint32_t Capacity>
class EntitySet
{


private:
    std::array<Entity, Capacity> entities { };
    std::array<Entity, Capacity> indexToEntity { };
    std::array<std::uint32_t, Capacity> entityToIndex { };

    std::uint32_t entityCount = 0;
};

//TODO: Fix all virtual deconstructors