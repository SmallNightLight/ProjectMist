#pragma once

#include "../../Math/FixedTypes.h"
#include "../../Math/Stream.h"

struct Transform
{
    Vector2 Position;
    Fixed16_16 Rotation; //Radians
    bool TransformUpdateRequired;
    bool AABBUpdateRequired;
    bool Changed;   //Non-persistent flag for caching

    inline Transform() noexcept = default;

    constexpr inline explicit Transform(Vector2 position, Fixed16_16 rotation) :
        Position(position), Rotation(rotation), TransformUpdateRequired(true), AABBUpdateRequired(true), Changed(true) { }

    inline explicit Transform(Stream& stream)
    {
        //Read object data
        Position = stream.ReadVector2();
        Rotation = stream.ReadFixed();

        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
        Changed = true;
    }

    Vector2 TransformVector(const Vector2 vector) const
    {
        Fixed16_16 sin = fpm::sin(Rotation);
        Fixed16_16 cos = fpm::cos(Rotation);

        return Vector2(cos * vector.X - sin * vector.Y + Position.X, sin * vector.X + cos * vector.Y + Position.Y);
    }

    void MovePosition(Vector2 direction)
    {
        Position += direction;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
    }

    void SetPosition(Vector2 position)
    {
        Position = position;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
    }

    void Rotate(Fixed16_16 amount)
    {
        Rotation += amount;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
    }

    void SetRotation(Fixed16_16 angle)
    {
        Rotation = angle;
        TransformUpdateRequired = true;
        AABBUpdateRequired = true;
    }

    void Serialize(Stream& stream) const
    {
        //Write transform data
        stream.WriteVector2(Position);
        stream.WriteFixed(Rotation);
    }

    inline constexpr bool operator==(const Transform& other) const noexcept
    {
        return Position == other.Position && Rotation == other.Rotation;
    }

    inline constexpr bool operator!=(const Transform& other) const noexcept
    {
        return Position != other.Position || Rotation != other.Rotation;
    }
};