#pragma once

#include <glm/glm.hpp>

struct Rect
{
    glm::vec2 Position;
    glm::vec2 Size;

    constexpr Rect() : Position(0.0f, 0.0f), Size(1.0f, 1.0f) { }
    explicit constexpr Rect(const glm::vec2& position, const glm::vec2& size = {1.0f, 1.0f}) : Position(position), Size(size) { }

    [[nodiscard]] constexpr bool Contains(const glm::vec2& point) const
    {
        return !(point.x < Position.x || point.y < Position.y || point.x >= (Position.x + Size.x) || point.y >= (Position.y + Size.y));
    }

    [[nodiscard]] constexpr bool Contains(const Rect& rect) const
    {
        return (rect.Position.x >= Position.x) && (rect.Position.x + rect.Size.x < Position.x + Size.x) &&
        (rect.Position.y >= Position.y) && (rect.Position.y + rect.Size.y < Position.y + Size.y);
    }

    [[nodiscard]] constexpr  bool Overlaps(const Rect& rect) const
    {
        return (Position.x < rect.Position.x + rect.Size.x && Position.x + Size.x >= rect.Position.x &&
        Position.y < rect.Position.y + rect.Size.y && Position.y + Size.y >= rect.Position.y);
    }
};