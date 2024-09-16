#pragma once

#include "./Math/fixed.hpp"

#include <bitset>
#include <cstdint>

using byte = std::uint8_t;

using Entity = std::uint32_t;
using ComponentType = std::uint8_t;

const std::uint32_t MAXENTITIES = 2000;
const ComponentType MAXCOMPONENTS = 32;

using Signature = std::bitset<MAXCOMPONENTS>;

using Fixed8_8 = fpm::fixed<std::int16_t, std::int32_t, 8>;
using Fixed16_16 = fpm::fixed<std::int32_t, std::int64_t, 16>;