#pragma once

#include "../../Math/FixedTypes.h"
#include "../../Math/Stream.h"

struct TransformBase
{
    Vector2 Position;
    Fixed16_16 Rotation; //Radians
};

struct TransformKey
{
    uint32_t Key1;
    uint32_t Key2;
    uint32_t Key3;

    inline constexpr bool operator==(const TransformKey& other) const noexcept
    {
        return Key1 == other.Key1 && Key2 == other.Key2  && Key3 == other.Key3;
    }

    inline constexpr bool operator!=(const TransformKey& other) const noexcept
    {
        return Key1 != other.Key1 || Key2 != other.Key2 || Key3 != other.Key3;
    }
};

struct Transform
{
    union
    {
        TransformBase Base;
        TransformKey Key;
    };

    bool TransformUpdateRequired;
    bool AABBUpdateRequired;
    bool Changed;   //Non-persistent flag for caching

    inline Transform() noexcept = default;

    constexpr inline explicit Transform(Vector2 position, Fixed16_16 rotation) :
        Base {position, rotation}, TransformUpdateRequired(true), AABBUpdateRequired(true), Changed(true) { }

    inline explicit Transform(Stream& stream)
    {
        //Read object data
        Base.Position = stream.ReadVector2();
        Base.Rotation = stream.ReadFixed();

        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
        Changed = true;
    }

    Vector2 TransformVector(const Vector2 vector) const
    {
        Fixed16_16 sin = fpm::sin(Base.Rotation);
        Fixed16_16 cos = fpm::cos(Base.Rotation);

        return Vector2(cos * vector.X - sin * vector.Y + Base.Position.X, sin * vector.X + cos * vector.Y + Base.Position.Y);
    }

    void MovePosition(Vector2 direction)
    {
        Base.Position += direction;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
    }

    void SetPosition(Vector2 position)
    {
        Base.Position = position;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
    }

    void Rotate(Fixed16_16 amount)
    {
        Base.Rotation += amount;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
    }

    void SetRotation(Fixed16_16 angle)
    {
        Base.Rotation = angle;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
    }

    void Serialize(Stream& stream) const
    {
        //Write transform data
        stream.WriteVector2(Base.Position);
        stream.WriteFixed(Base.Rotation);
    }
};