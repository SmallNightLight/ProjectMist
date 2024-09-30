#pragma once

#include <glm/glm.hpp>
#include <limits>
#include <algorithm>

static constexpr float infinity = std::numeric_limits<float>::infinity();

struct Box
{
    glm::vec2 min{infinity, infinity};
    glm::vec2 max{-infinity, -infinity};

    Box& operator |= (glm::vec2 const& point)
    {
        min.x = std::min(min.x, point.x);
        min.y = std::min(min.y, point.y);
        max.x = std::max(max.x, point.x);
        max.y = std::max(max.y, point.y);
        return *this;
    }

    template<typename Iterator>
    Box bbox(Iterator begin, Iterator end)
    {
        Box result;
        for(auto it = begin; it != end; it++)
            result |= *it;

        return result;
    }
};