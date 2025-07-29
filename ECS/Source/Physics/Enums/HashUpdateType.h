#pragma once

#include <cstdint>

enum class HashUpdateType : uint8_t
{
    None,
    BaseUpdate,
    FullUpdate
};