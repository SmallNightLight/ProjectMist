#pragma once

#include "../../Math/FixedTypes.h"
#include "../../Math/Stream.h"
#include "Transform.h"
#include "TransformMeta.h"

#include <array>

inline static constexpr uint8_t MaxVertices = 8;

//PolygonCollider cannot change shape after it has been created
class PolygonCollider
{
public:
    inline PolygonCollider() noexcept = default;

    constexpr inline explicit PolygonCollider(const std::array<Vector2, MaxVertices>& vertices, uint8_t vertexCount) : Vertices(vertices), TransformedVertices(Vertices), VertexCount(vertexCount) { }

    template<typename Container>
    constexpr inline explicit PolygonCollider(const Container& vertices)
    {
        VertexCount = static_cast<uint8_t>(std::min<size_t>(vertices.size(), MaxVertices));

        std::copy_n(vertices.begin(), VertexCount, Vertices.begin());

        //Fill empty space with zero
        if (VertexCount < MaxVertices)
        {
            std::fill(Vertices.begin() + VertexCount, Vertices.end(), Vector2(0, 0));
        }

        TransformedVertices = Vertices;
    }

    inline explicit PolygonCollider(Stream& stream)
    {
        //Read vertices count
        uint8_t maxCount = stream.ReadInteger<uint8_t>();
        VertexCount = stream.ReadInteger<uint8_t>();

        assert(maxCount == MaxVertices && VertexCount <= MaxVertices);

        //Read Vertices
        for (int i = 0; i < VertexCount; ++i)
        {
            Vertices[i] = stream.ReadVector2();
        }

        //Read Transformed Vertices
        for (int i = 0; i < VertexCount; ++i)
        {
            TransformedVertices[i] = stream.ReadVector2();
        }
    }

    void Serialize(Stream& stream) const
    {
        //Write vertex count
        stream.WriteInteger<uint8_t>(MaxVertices);
        stream.WriteInteger<uint8_t>(VertexCount);

        //Write Vertices
        for (uint32_t i = 0; i < VertexCount; ++i)
        {
            stream.WriteVector2(Vertices[i]);
        }

        //Write Transformed Vertices
        for (uint32_t i = 0; i < VertexCount; ++i)  //todo should write either vertices or transformed
        {
            stream.WriteVector2(TransformedVertices[i]);
        }
    }

    constexpr inline Vector2Span GetDefaultVertices()
    {
        return Vector2Span(Vertices.data(), VertexCount);
    }

    Vector2Span GetTransformedVertices(Transform& transform)
    {
        if (transform.TransformUpdateRequired)
        {
            for (uint32_t i = 0; i < VertexCount; ++i)
            {
                TransformedVertices[i] = transform.TransformVector(Vertices[i]);
            }

            transform.TransformUpdateRequired = false;
        }

        return Vector2Span(TransformedVertices.data(), VertexCount);
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
    std::array<Vector2, MaxVertices> Vertices;
    std::array<Vector2, MaxVertices> TransformedVertices;

    uint8_t VertexCount;
};