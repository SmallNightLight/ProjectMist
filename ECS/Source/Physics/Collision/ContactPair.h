#pragma once

#include "../../ECS/ECSSettings.h"
#include "../../Math/FixedTypes.h"

struct VelocityData
{
    EntityPair EntityKey;
    Vector2 Velocity;
    Fixed16_16 AngularVelocity;
    Fixed16_16 Pn;
    Fixed16_16 Pt;

    friend inline constexpr bool operator==(const VelocityData& left, const EntityPair& right) noexcept
    {
        return left.EntityKey == right;
    }

    friend inline constexpr bool operator<(const VelocityData& left, const VelocityData& right) noexcept
    {
        return left.EntityKey < right.EntityKey;
    }

    friend inline constexpr bool operator<(const EntityPair& left, const VelocityData& right) noexcept
    {
        return left < right.EntityKey;
    }
};

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
    FeaturePair Feature;
};

struct ImpulseData
{
    ImpulseData() = default;

    EntityPair EntityKey;
    std::array<AccumulatedImpulse, 2> LastImpulses;
    uint8_t ContactCount;

    friend inline constexpr bool operator==(const ImpulseData& left, const EntityPair& right) noexcept
    {
        return left.EntityKey == right;
    }

    friend inline constexpr bool operator<(const ImpulseData& left, const ImpulseData& right) noexcept
    {
        return left.EntityKey < right.EntityKey;
    }

    friend inline constexpr bool operator<(const EntityPair& left, const ImpulseData& right) noexcept
    {
        return left < right.EntityKey;
    }
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

    EntityPair EntityKey;
    Entity Entity1;
    Entity Entity2;

    std::array<Contact, 2> Contacts;
    uint8_t ContactCount;

    Vector2 Normal;
    Fixed16_16 Friction;

    friend inline constexpr bool operator==(const ContactPair& left, const EntityPair& right) noexcept
    {
        return left.EntityKey == right;
    }

    friend inline constexpr bool operator<(const ContactPair& left, const ContactPair& right) noexcept
    {
        return left.EntityKey < right.EntityKey;
    }

    friend inline constexpr bool operator<(const EntityPair& left, const ContactPair& right) noexcept
    {
        return left < right.EntityKey;
    }
};

// struct ContactPairSIMD
// {
//     ContactPairSIMD() = default;
//
//     EntityPair EntityKey;
//     Entity Entity1;
//     Entity Entity2;
//
//     std::array<Contact, 2> Contacts;
//     uint8_t ContactCount;
//
//     Vector2 Normal;
//     Fixed16_16 Friction;
// };