#pragma once

#include <bitset>
#include <cstdint>

using byte = std::uint8_t;

using Entity = std::uint32_t;
using ComponentType = std::uint8_t;

const Entity MAXENTITIES = 2000;
const ComponentType MAXCOMPONENTS = 32;

using Signature = std::bitset<MAXCOMPONENTS>;