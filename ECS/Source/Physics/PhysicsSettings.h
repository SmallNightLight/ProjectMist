#pragma once

#include "../Math/FixedTypes.h"

constexpr uint8_t PhysicsIterations = 5;

constexpr int64_t VelocityEpsilon = (Fixed16_16(1) / Fixed16_16(1000)).GetValueIntermediate();
constexpr Fixed16_16 AngularVelocityEpsilon = Fixed16_16(1) / Fixed16_16(100);

constexpr Fixed16_16 MinBodySize = Fixed16_16::FromFloat(0.01f * 0.01f);
constexpr Fixed16_16 MaxBodySize( 64 * 64);

constexpr Fixed16_16 MinDensity = Fixed16_16::FromFloat( 0.5f);
constexpr Fixed16_16 MaxDensity = Fixed16_16::FromFloat( 21.4f);

constexpr  Vector2 Gravity = Vector2(Fixed16_16(0), Fixed16_16(-9, 81));

using CollisionHash = std::uint32_t; //TODO

//Debug
constexpr bool PhysicsDebugMode = true;
constexpr bool LogCollisions = false;