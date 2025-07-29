#pragma once

#include "../Math/FixedTypes.h"

constexpr int PhysicsIterations = 5;

constexpr std::int32_t QuantizationStepPosition = 2; //in Log2 (2^x)
constexpr std::int32_t QuantizationStepRotation = 1; //in Log2 (2^x)

constexpr Fixed16_16 MinBodySize = Fixed16_16::FromFloat(0.01f * 0.01f);
constexpr Fixed16_16 MaxBodySize( 64 * 64);

constexpr Fixed16_16 MinDensity = Fixed16_16::FromFloat( 0.5f);
constexpr Fixed16_16 MaxDensity = Fixed16_16::FromFloat( 21.4f);

constexpr  Vector2 Gravity = Vector2(Fixed16_16(0), Fixed16_16(-9, 81));

using CollisionHash = std::uint32_t;

//Debug
constexpr bool PhysicsDebugMode = false;
constexpr bool LogCollisions = false;