#pragma once

#include "../../Math/FixedTypes.h"
#include "../../Math/Stream.h"

#include <array>

//BoxCollider cannot change shape after it has been created
class BoxCollider
{
public:
    inline BoxCollider() noexcept = default;

    constexpr inline explicit BoxCollider(Fixed16_16 width, Fixed16_16 height) : Width(width), Height(height), Vertices(GetBoxVertices(width, height)), TransformedVertices(Vertices) { }

    inline explicit BoxCollider(Stream& stream)
    {
        //Read Size
        Width = stream.ReadFixed();
        Height = stream.ReadFixed();

        //Read Vertices
        for (int i = 0; i < 4; ++i)
        {
            Vertices[i] = stream.ReadVector2();
        }

        //Read Transformed Vertices
        for (int i = 0; i < 4; ++i)
        {
            TransformedVertices[i] = stream.ReadVector2();
        }
    }

    void Serialize(Stream& stream) const
    {
        //Write Size
        stream.WriteFixed(Width);
        stream.WriteFixed(Height);

        //Write Vertices
        for (const auto& vertex : Vertices)
        {
            stream.WriteVector2(vertex);
        }

        //Write Transformed Vertices
        for (const auto& vertex : TransformedVertices)
        {
            stream.WriteVector2(vertex);
        }
    }

    constexpr inline Vector2Span GetVertices()
    {
        return Vector2Span(Vertices.data(), 4);
    }

    constexpr inline Vector2Span GetTransformedVertices()
    {
        return Vector2Span(TransformedVertices.data(), 4);
    }

private:
    static constexpr std::array<Vector2, 4> GetBoxVertices(Fixed16_16 width, Fixed16_16 height)
    {
        Fixed16_16 left = -width / 2;
        Fixed16_16 right = left + width;
        Fixed16_16 bottom = -height / 2;
        Fixed16_16 top = bottom + height;

        return std::array<Vector2, 4>
        {
            Vector2(left, bottom),
            Vector2(right, bottom),
            Vector2(right, top),
            Vector2(left, top)
        };
    }

private:
    Fixed16_16 Width;
    Fixed16_16 Height;

    std::array<Vector2, 4> Vertices;
    std::array<Vector2, 4> TransformedVertices;
};