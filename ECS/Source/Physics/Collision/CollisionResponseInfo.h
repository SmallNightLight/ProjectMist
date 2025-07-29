#pragma once

#include "../../Math/FixedTypes.h"
#include "../../ECS/ECSSettings.h"

struct CollisionResponseInfo
{
    constexpr inline CollisionResponseInfo(bool collision, Vector2 velocity, Fixed16_16 angularVelocity) :
        Collision(collision),
        Velocity(velocity),
        AngularVelocity(angularVelocity) { }

    bool Collision;
    Vector2 Velocity;
    Fixed16_16 AngularVelocity;
};

static constexpr CollisionResponseInfo NonColliding = CollisionResponseInfo(false, Vector2::Zero(), Fixed16_16(0));