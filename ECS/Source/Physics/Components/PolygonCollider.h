#pragma once

struct PolygonCollider
{
    std::vector<Vector2> Vertices { };
    std::vector<Vector2> TransformedVertices { };

    PolygonCollider() = default;
    explicit PolygonCollider(const std::vector<Vector2>& vertices) : Vertices(vertices), TransformedVertices(Vertices) { }
    explicit PolygonCollider(Stream& stream)
    {
        //Read Vertices
        uint8_t verticesSize = stream.ReadInteger<uint8_t>();
        Vertices.resize(verticesSize);
        for (int i = 0; i < verticesSize; ++i)
        {
            Vertices[i] = stream.ReadVector2();
        }

        //Read Transformed Vertices
        uint8_t transformedVerticesSize = stream.ReadInteger<uint8_t>();
        TransformedVertices.resize(transformedVerticesSize);
        for (int i = 0; i < transformedVerticesSize; ++i)
        {
            TransformedVertices[i] = stream.ReadVector2();
        }
    }

    void Serialize(Stream& stream) const
    {
        //Write Vertices
        stream.WriteInteger<uint8_t>(static_cast<uint8_t>(Vertices.size()));
        for (const auto& vertex : Vertices)
        {
            stream.WriteVector2(vertex);
        }

        //Write Transformed Vertices
        stream.WriteInteger<uint8_t>(static_cast<uint8_t>(TransformedVertices.size()));
        for (const auto& vertex : TransformedVertices)
        {
            stream.WriteVector2(vertex);
        }
    }
};