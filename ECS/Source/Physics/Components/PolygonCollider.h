#pragma once

struct PolygonCollider
{
    std::vector<Vector2> Vertices { };
    std::vector<Vector2> TransformedVertices { };

    PolygonCollider() = default;
    explicit PolygonCollider(const std::vector<Vector2>& vertices) : Vertices(vertices), TransformedVertices(Vertices) { }
};