#include "Base/ECSManager.h"
#include "Components/ComponentHeaders.h"
#include "Systems/SystemHeader.h"


#include <iostream>
#include <chrono>
#include <GLFW/glfw3.h>

ECSManager EcsManager;


int main()
{
    //ECS setup
    EcsManager.Setup();

    //Register components
    EcsManager.RegisterComponent<Transform>();
    EcsManager.RegisterComponent<Velocity>();
    EcsManager.RegisterComponent<ParticleData>();
    EcsManager.RegisterComponent<RectangleData>();
    EcsManager.RegisterComponent<MovingParticleData>();

    //Register systems
    auto movementSystem = EcsManager.RegisterSystem<Movement>();
    auto particleRenderer = EcsManager.RegisterSystem<ParticleRenderer>();
    auto rectangleRenderer = EcsManager.RegisterSystem<RectangleRenderer>();
    auto movingParticleRenderer = EcsManager.RegisterSystem<MovingParticleRenderer>();

    //Setup signatures
    EcsManager.SetSignature<Movement>(Movement::GetSignature());
    EcsManager.SetSignature<ParticleRenderer>(ParticleRenderer::GetSignature());
    EcsManager.SetSignature<RectangleRenderer>(RectangleRenderer::GetSignature());
    EcsManager.SetSignature<MovingParticleRenderer>(MovingParticleRenderer::GetSignature());

    std::default_random_engine random;

    //Add entities
    for (Entity entity = 0; entity < MAXENTITIES; ++entity)
    {
        EcsManager.CreateEntity();

        std::uniform_real_distribution<float> randomPosition(0.0f, 700.0f);
        std::uniform_real_distribution<float> randomSize(1.0f, 30.0f);
        std::uniform_int_distribution<int> randomColor(0, 255);

        //Particles with split components
        /**/
        EcsManager.AddComponent(entity, Transform {randomPosition(random), randomPosition(random)});
        EcsManager.AddComponent(entity, Velocity {0, 0});
        EcsManager.AddComponent(entity, ParticleData { randomSize(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random)});
        //EcsManager.AddComponent(entity, RectangleData{ randomSize(random),randomSize(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random)});
        /**/

        //Particles with one component and one system that handles movement and rendering
        /**
        EcsManager.AddComponent(entity, MovingParticleData {randomPosition(random), randomPosition(random), 0, 0, randomSize(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random)});
        /**/
    }

    //Initialize glfw3
    if (!glfwInit())
    {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(700, 700, "Rendering Test", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    //Set vsync (0 = disabled)
    glfwSwapInterval( 0 );


    //Set up the viewport
    glViewport(0, 0, 700, 700);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 700, 0, 700, -1, 1);


    int frameCount = 0;
    auto start = std::chrono::steady_clock::now();


    while (!glfwWindowShouldClose(window))
    {
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
        glClear(GL_COLOR_BUFFER_BIT);
        movementSystem->Update();
        particleRenderer->Render();
        rectangleRenderer->Render();
        movingParticleRenderer->Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}