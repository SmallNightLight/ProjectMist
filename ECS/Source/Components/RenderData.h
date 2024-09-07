#pragma once

#include <SFML/Graphics.hpp>

struct RendererData
{
    sf::Vector2f Position;
    sf::Vector2f Size;
    sf::Color Color;

    RendererData() = default;
    RendererData(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Color& color) : Position(position), Size(size), Color(color) { }
};