#pragma once

#include "../../ECS/ECSSettings.h"
#include "../../Math/FixedTypes.h"

union FeaturePair
{
    struct Edges
    {
        uint8_t ReferenceEdge;
        uint8_t IncidentEdge;
        uint16_t Flipped;
    } edges;

    uint32_t value;
};

struct AccumulatedImpulse
{
    AccumulatedImpulse() = default;

    Fixed16_16 Pn;  //Accumulated normal impulse
    Fixed16_16 Pt;  //Accumulated tangent impulse
    Fixed16_16 Pnb; //Accumulated normal impulse for position bias
};

struct Contact
{
    Contact() = default;

    Vector2 Position;
    Vector2 R1, R2;
    AccumulatedImpulse LastImpulses;
    Fixed16_16 Separation;
    Fixed16_16 MassNormal, MassTangent;
    FeaturePair Feature;
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