#pragma once

#include "../../Math/Stream.h"

#include <vector>

//BoxCollider cannot change shape after it has been created
class BoxCollider
{
public:
    BoxCollider() : Width(0), Height(0) { }

    BoxCollider(Fixed16_16 width, Fixed16_16 height) : Vertices(GetBoxVertices(width, height)), TransformedVertices(Vertices), Width(width), Height(height) { }

    explicit BoxCollider(Stream& stream)
    {
        //Read Size
        Width = stream.ReadFixed();
        Height = stream.ReadFixed();

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
        //Write Size
        stream.WriteFixed(Width);
        stream.WriteFixed(Height);

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

private:
    [[nodiscard]] static std::vector<Vector2> GetBoxVertices(Fixed16_16 width, Fixed16_16 height)
    {
        Fixed16_16 left = -width / 2;
        Fixed16_16 right = left + width;
        Fixed16_16 bottom = -height / 2;
        Fixed16_16 top = bottom + height;

        return std::vector
        {
            Vector2{left, top},
            Vector2{right, top},
            Vector2{right, bottom},
            Vector2{left, bottom}
        };
    }

public:
    std::vector<Vector2> Vertices { };
    std::vector<Vector2> TransformedVertices { };

private:
    Fixed16_16 Width;
    Fixed16_16 Height;
};