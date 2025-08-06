#pragma once

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
        for (uint32_t i = 0; i < VertexCount; ++i)
        {
            stream.WriteVector2(TransformedVertices[i]);
        }
    }

    constexpr inline Vector2Span GetVertices()
    {
        return Vector2Span(Vertices.data(), VertexCount);
    }

    constexpr inline Vector2Span GetTransformedVertices()
    {
        return Vector2Span(TransformedVertices.data(), VertexCount);
    }

private:
    std::array<Vector2, MaxVertices> Vertices;
    std::array<Vector2, MaxVertices> TransformedVertices;

    uint8_t VertexCount;
};