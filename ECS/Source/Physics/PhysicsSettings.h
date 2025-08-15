#pragma once

#include "../Math/FixedTypes.h"

constexpr uint8_t PhysicsIterations = 10;

constexpr int64_t VelocityEpsilon = (Fixed16_16(1) / Fixed16_16(1000)).GetValueIntermediate();
constexpr Fixed16_16 AngularVelocityEpsilon = Fixed16_16(1) / Fixed16_16(100);

constexpr  Vector2 Gravity = Vector2(Fixed16_16(0), Fixed16_16(-10));

using CollisionHash = std::uint32_t; //TODO

//Debug
constexpr bool PhysicsDebugMode = true;
constexpr bool LogCollisions = false;