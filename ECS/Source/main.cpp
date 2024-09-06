#include "Base/ECSManager.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Test Window");

    int frameCount = 0;
    auto start = std::chrono::steady_clock::now();

    while (window.isOpen())
    {
        sf::Event event;
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

        sf::RectangleShape rectangle(sf::Vector2f(200.0f, 100.0f));
        rectangle.setPosition(300.0f, 250.0f);
        rectangle.setFillColor(sf::Color::Green);

        //Render
        window.clear(); // Clear the window with the current clear color
        window.draw(rectangle);
        window.display();
    }

    return 0;
}