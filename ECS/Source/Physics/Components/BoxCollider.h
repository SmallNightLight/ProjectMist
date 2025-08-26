#pragma once

#include "../../Math/FixedTypes.h"
#include "../../Math/Stream.h"
#include "Transform.h"
#include "TransformMeta.h"

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

    constexpr inline Vector2Span GetDefaultVertices()
    {
        return Vector2Span(Vertices.data(), 4);
    }

    Vector2Span GetTransformedVertices(Transform& transform)
    {
        if (transform.TransformUpdateRequired)
        {
            for (uint32_t i = 0; i < 4; ++i)
            {
                TransformedVertices[i] = transform.TransformVector(Vertices[i]);
            }

            transform.TransformUpdateRequired = false;
        }

        return Vector2Span(TransformedVertices.data(), 4);
    }

    const AABB& GetAABB(Transform& transform, TransformMeta& transformMeta)
    {
        if (transform.AABBUpdateRequired)
        {
            Fixed16_16 minX = std::numeric_limits<Fixed16_16>::max();
            Fixed16_16 minY = std::numeric_limits<Fixed16_16>::max();;
            Fixed16_16 maxX = std::numeric_limits<Fixed16_16>::min();;
            Fixed16_16 maxY = std::numeric_limits<Fixed16_16>::min();;

            Vector2Span transformedVertices = GetTransformedVertices(transform);
            for(Vector2 vertex : transformedVertices)
            {
                if (vertex.X < minX) { minX = vertex.X; }
                if (vertex.X > maxX) { maxX = vertex.X; }
                if (vertex.Y < minY) { minY = vertex.Y; }
                if (vertex.Y > maxY) { maxY = vertex.Y; }
            }

            transformMeta.BoundingBox = AABB(Vector2(minX, minY), Vector2(maxX, maxY));
            transform.AABBUpdateRequired = false;
        }

        return transformMeta.BoundingBox;
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
    std::array<Vector2, 4> TransformedVertices; //todo to meta??
};