#include "Base/ECSManager.h"
#include "Components/ComponentHeaders.h"
#include "Systems/SystemHeader.h"


#include <iostream>
#include <sstream>
#include <GLFW/glfw3.h>

ECSManager EcsManager;

double GetDeltaTime(double& lastTime)
{
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    return deltaTime;
}

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


        EcsManager.AddComponent(entity, Transform {randomPosition(random), randomPosition(random)});
        EcsManager.AddComponent(entity, Velocity {0, 0});
        EcsManager.AddComponent(entity, ParticleData { randomSize(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random)});
        //EcsManager.AddComponent(entity, RectangleData{ randomSize(random),randomSize(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random)});



        //Particles with one component and one system that handles movement and rendering
        //EcsManager.AddComponent(entity, MovingParticleData {randomPosition(random), randomPosition(random), 0, 0, randomSize(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random)});
    }

    //Initialize glfw3
    if (!glfwInit())
    {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(700, 700, "ECS Test", nullptr, nullptr);
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


    double lastTime = 0.0;
    double lastTitleUpdateTime = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        //Calculate delta time
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (currentTime - lastTitleUpdateTime >= 1.0f)
        {
            std::ostringstream title;
            title << "ECS Test - FPS: " << static_cast<int>(1.0 / deltaTime);
            glfwSetWindowTitle(window, title.str().c_str());
            lastTitleUpdateTime = currentTime;
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