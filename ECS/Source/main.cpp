#include "Base/ECSManager.h"
#include "Components/ComponentHeaders.h"
#include "Systems/SystemHeader.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>

ECSManager EcsManager;

int main()
{
    //ECS setup
    EcsManager.Setup();

    //Register components
    EcsManager.RegisterComponent<RendererData>();

    //Register systems
    auto rendererComponent = EcsManager.RegisterSystem<Renderer>();

    //Setup signatures
    EcsManager.SetSignature<Renderer>(Renderer::GetSignature());

    std::default_random_engine random;

    //Add entities
    for (Entity entity = 0; entity < MAXENTITIES; ++entity)
    {
        EcsManager.CreateEntity();

        std::uniform_real_distribution<float> randomPosition(0.0f, 700.0f);
        std::uniform_real_distribution<float> randomSize(1.0f, 30.0f);
        std::uniform_int_distribution<int> randomColor(0, 255);

        EcsManager.AddComponent(entity, RendererData{sf::Vector2f (randomPosition(random), randomPosition(random)), sf::Vector2f (randomSize(random), randomSize(random)), sf::Color(randomColor(random), randomColor(random), randomColor(random))});
    }

    //Initialize SFML
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML Test Window");
    int frameCount = 0;
    auto start = std::chrono::steady_clock::now();

    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(16));

        frameCount++;

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - start;

        double dt = 1.0 / frameCount;

        if (elapsed.count() >= 1.0)
        {
            std::cout << "FPS: " << frameCount << "   Delta: " << dt << std::endl;

            frameCount = 0;
            start = now;
        }


        //Render
        window.clear(); // Clear the window with the current clear color
        rendererComponent->Render(window);
        window.display();
    }

    return 0;
}