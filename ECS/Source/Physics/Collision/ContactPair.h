#pragma once

#include "../../ECS/ECSSettings.h"
#include "../../Math/FixedTypes.h"

#include <type_traits>

union FeaturePair
{
    struct Edges
    {
        uint8_t ReferenceEdge;
        uint8_t IncidentEdge;
        uint16_t Flipped;
    } edges;

    uint32_t Value;
};

struct AccumulatedImpulse
{
    AccumulatedImpulse() = default;

    Fixed16_16 Pn;  //Accumulated normal impulse
    Fixed16_16 Pt;  //Accumulated tangent impulse
    Fixed16_16 Pnb; //Accumulated normal impulse for position bias
    FeaturePair Feature;
};

struct ImpulseData
{
    ImpulseData() = default;

    EntityPair EntityKey;
    std::array<AccumulatedImpulse, 2> LastImpulses;
    uint8_t ContactCount;
};

struct Contact
{
    Contact() = default;

    Vector2 Position;
    Vector2 R1, R2;
    Fixed16_16 Separation;
    Fixed16_16 MassNormal, MassTangent;
    AccumulatedImpulse LastImpulse;
};

struct ContactPair
{
    ContactPair() = default;

    Entity Entity1;
    Entity Entity2;

    std::array<Contact, 2> Contacts;
    uint8_t ContactCount;

    Vector2 Normal;
    Fixed16_16 Friction;
};

static_assert(std::is_trivially_default_constructible_v<ImpulseData>, "Impulse cache needs to be trivial");