#pragma once

#include <cstdint>

enum class HashType : uint16_t
{
    None                = 0,

    //Basic data
    Position            = 1 << 0,
    Rotation            = 1 << 1,
    Velocity            = 1 << 2,
    AngularVelocity     = 1 << 3,

    //Shape data
    Scale               = 1 << 4,
    Shape               = 1 << 5,

    //RigidBody data
    Mass                = 1 << 6,
    Restitution         = 1 << 7,
    Inertia             = 1 << 8,
    Friction            = 1 << 9
};

//Operators for bitwise use
constexpr inline HashType operator|(HashType hash1, HashType hash2)
{
    return static_cast<HashType>(
            static_cast<uint16_t>(hash1) | static_cast<uint16_t>(hash2));
}

constexpr inline HashType operator&(HashType hash1, HashType hash2)
{
    return static_cast<HashType>(
            static_cast<uint16_t>(hash1) & static_cast<uint16_t>(hash2));
}

constexpr inline HashType& operator|=(HashType& hash1, HashType hash2)
{
    hash1 = hash1 | hash2;
    return hash1;
}

constexpr inline HashType& operator&=(HashType& hash1, HashType hash2)
{
    hash1 = hash1 & hash2;
    return hash1;
}

constexpr inline HashType operator~(HashType hash)
{
    return static_cast<HashType>(
            ~static_cast<uint16_t>(hash));
}

static constexpr HashType NoHashType = HashType::None;
static constexpr HashType KinematicHashType = HashType::Position | HashType::Rotation;
static constexpr HashType DynamicHashType = HashType::Position | HashType::Rotation | HashType::Velocity | HashType::AngularVelocity;
static constexpr HashType CompleteHashType = HashType::Position | HashType::Rotation | HashType::Velocity | HashType::AngularVelocity | HashType::Scale | HashType::Shape | HashType::Mass | HashType::Restitution | HashType::Inertia | HashType::Friction;