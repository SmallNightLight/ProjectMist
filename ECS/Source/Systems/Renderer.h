#pragma once

#include <SFML/Graphics.hpp>
#include <random>

extern ECSManager EcsManager;

class Renderer : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<RendererData>());
        return signature;
    }

    void Render(sf::RenderWindow& window)
    {
        sf::VertexArray vertices(sf::Quads);
        for (const Entity& entity : Entities)
        {
            auto& renderData = EcsManager.GetComponent<RendererData>(entity);

            //Create vertices for the rectangle
            sf::Vertex v1(renderData.Position, renderData.Color);
            sf::Vertex v2(sf::Vector2f(renderData.Position.x + renderData.Size.x, renderData.Position.y), renderData.Color);
            sf::Vertex v3(sf::Vector2f(renderData.Position.x + renderData.Size.x, renderData.Position.y + renderData.Size.y), renderData.Color);
            sf::Vertex v4(sf::Vector2f(renderData.Position.x, renderData.Position.y + renderData.Size.y), renderData.Color);

            vertices.append(v1);
            vertices.append(v2);
            vertices.append(v3);
            vertices.append(v4);
        }

        window.draw(vertices);
    }
};