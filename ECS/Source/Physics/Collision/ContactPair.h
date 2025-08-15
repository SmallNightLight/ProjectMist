#pragma once

#include "../../ECS/ECSSettings.h"
#include "../../Math/FixedTypes.h"

union FeaturePair
{
    struct Edges
    {
        uint8_t inEdge1;
        uint8_t outEdge1;
        uint8_t inEdge2;
        uint8_t outEdge2;
    } e;

    uint32_t value;
};

struct Contact
{
    Contact() = default;

    Vector2 Position;
    Vector2 R1, R2;
    Fixed16_16 Separation;
    Fixed16_16 Pn;	                    //Accumulated normal impulse
    Fixed16_16 Pt;	                    //Accumulated tangent impulse
    Fixed16_16 Pnb;	                    //Accumulated normal impulse for position bias
    Fixed16_16 MassNormal, MassTangent;
    Fixed16_16 Bias;
    FeaturePair Feature;
};

struct ContactPair
{
    Entity Entity1;
    Entity Entity2;

    uint8_t ContactCount;
    std::array<Contact, 2> Contacts;

    Vector2 Normal;
    Fixed16_16 Friction = Fixed16_16(1) / Fixed16_16(5);

    bool RefIsPoly1;
    uint8_t  RefEdgeIndex;// todo
    Fixed16_16 Penetration;
};