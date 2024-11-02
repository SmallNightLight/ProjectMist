#pragma once

#include <array>

struct BoxCollider
{
    Fixed16_16 Width;
    Fixed16_16 Height;

    std::vector<Vector2> Vertices { };
    std::vector<Vector2> TransformedVertices { };

    BoxCollider() : Width(0), Height(0) { }
    BoxCollider(Fixed16_16 width, Fixed16_16 height) : Width(width), Height(height), Vertices(GetBoxVertices()), TransformedVertices(Vertices) { }

    std::vector<Vector2> GetTransformedVertices(ColliderTransform& colliderTransform)
    {
        if (colliderTransform.TransformUpdateRequired)
        {
            for (int i = 0; i < TransformedVertices.size(); ++i)
            {
                TransformedVertices[i] = colliderTransform.Transform(Vertices[i]);
            }

            colliderTransform.TransformUpdateRequired = false;
        }

        return TransformedVertices;
    }

private:
    [[nodiscard]] std::vector<Vector2> GetBoxVertices() const
    {
        Fixed16_16 left = -Width / 2;
        Fixed16_16 right = left + Width;
        Fixed16_16 bottom = -Height / 2;
        Fixed16_16 top = bottom + Height;

        return std::vector
        {
            Vector2{left, top},
            Vector2{right, top},
            Vector2{right, bottom},
            Vector2{left, bottom}
        };
    }

    /*static std::array<std::uint8_t, 6> GetBoxTriangles()
    {
        return std::array<std::uint8_t, 6> {0, 1, 2, 0, 2, 3};
    }*/
};