#pragma once

#include "../Math/FixedTypes.h"

constexpr Fixed16_16 MinBodySize = Fixed16_16::FromFloat(0.01f * 0.01f);
constexpr Fixed16_16 MaxBodySize( 064 * 64);

constexpr Fixed16_16 MinDensity = Fixed16_16::FromFloat( 0.5f);
constexpr Fixed16_16 MaxDensity = Fixed16_16::FromFloat( 21.4f);