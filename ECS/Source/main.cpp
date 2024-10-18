#include "Base/ECSManager.h"
#include "Components/ComponentHeaders.h"
#include "Systems/SystemHeader.h"

#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Math/TestMath.h"

ECSManager EcsManager;

void SetFrameSize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Vector2 GetMousePosition(GLFWwindow* window)
{
    //Get the mouse position in pixel coordinates
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    //Convert mouse X and Y to normalized screen coordinates [-1, 1]
    return Vector2 {Fixed16_16::FromFloat<double>(mouseX), Fixed16_16::FromFloat<float>(SCREEN_HEIGHT - (float)mouseY) };
}

int main()
{
    //return TestMath::Test();

    //Initialize OpenGL
    if (!glfwInit())
    {
        return -1;
    }

    //Create adn initialize a new window
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Particle System - Instancing", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    //Disable vsync (0 = Disabled)
    glfwSwapInterval( 0 );

    //Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, SetFrameSize);

    //Set up the viewport
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1, 1);

    //ECS setup
    EcsManager.Setup();

    //Register components
    EcsManager.RegisterComponent<Transform>();
    EcsManager.RegisterComponent<Velocity>();
    EcsManager.RegisterComponent<Lifetime>();
    EcsManager.RegisterComponent<Boid>();

    //Register systems
    auto movementSystem = EcsManager.RegisterSystem<Movement>();
    auto particleRenderer = EcsManager.RegisterSystem<ParticleRenderer>();
    auto blinkingParticles = EcsManager.RegisterSystem<BlinkingParticles>();
    auto boidMovement = EcsManager.RegisterSystem<BoidMovement>();
    auto boidRenderer = EcsManager.RegisterSystem<BoidRenderer>();

    //Setup signatures
    EcsManager.SetSignature<Movement>(Movement::GetSignature());
    EcsManager.SetSignature<ParticleRenderer>(ParticleRenderer::GetSignature());
    EcsManager.SetSignature<BlinkingParticles>(BlinkingParticles::GetSignature());
    EcsManager.SetSignature<BoidMovement>(BoidMovement::GetSignature());
    EcsManager.SetSignature<BoidRenderer>(BoidRenderer::GetSignature());

    std::default_random_engine random;
    FixedRandom16_16 randomPositionX(Fixed16_16(0), Fixed16_16(SCREEN_WIDTH));
    FixedRandom16_16 randomPositionY(Fixed16_16(0), Fixed16_16(SCREEN_HEIGHT));
    FixedRandom16_16 randomVelocity(Fixed16_16(-1), Fixed16_16(1));
    FixedRandom16_16 randomLifetime(Fixed16_16(0), Fixed16_16(3));

    //Add entities
    for (Entity entity = 0; entity < MAXENTITIES; ++entity)
    {
        EcsManager.CreateEntity();

        EcsManager.AddComponent(entity, Transform {randomPositionX(random), randomPositionY(random)});
        //EcsManager.AddComponent(entity, Velocity {randomVelocity(random), randomVelocity(random)});
        //EcsManager.AddComponent(entity, Lifetime {randomLifetime(random)});
        EcsManager.AddComponent(entity, Boid {Vector2{randomVelocity(random), randomVelocity(random)}, Vector2{0, 0} });
    }

    bool isPaused = false;
    double lastTime = 0.0;
    double lastTitleUpdateTime = 0.0;

    while (!glfwWindowShouldClose(window))
    {
        //Calculate delta time
        double currentTime = glfwGetTime();
        auto deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        if (currentTime - lastTitleUpdateTime >= 1.0f)
        {
            std::ostringstream title;
            title << "ECS Test - FPS: " << static_cast<int>(1.0f / deltaTime);
            glfwSetWindowTitle(window, title.str().c_str());
            lastTitleUpdateTime = currentTime;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            isPaused = !isPaused;
            glfwWaitEventsTimeout(0.3);
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
            break;
        }

        //Render
        glClear(GL_COLOR_BUFFER_BIT);

        //Fixed16_16 fixedDelta = Fixed16_16(1) / Fixed16_16(60);
        if (!isPaused)
        {
            movementSystem->Update(Fixed16_16::FromFloat(deltaTime), GetMousePosition(window));
            boidMovement->Update(Fixed16_16::FromFloat(deltaTime));
            blinkingParticles->Update(Fixed16_16::FromFloat(deltaTime));
        }

        particleRenderer->Render();
        boidRenderer->Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //Stop glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}


//Main function to run with no window
/*
int main()
{
    //ECS setup
    EcsManager.Setup();

    //Register components
    EcsManager.RegisterComponent<Transform>();
    EcsManager.RegisterComponent<Velocity>();
    EcsManager.RegisterComponent<Boid>();

    //Register systems
    auto movementSystem = EcsManager.RegisterSystem<Movement>();
    auto boidMovement = EcsManager.RegisterSystem<BoidMovement>();

    //Setup signatures
    EcsManager.SetSignature<Movement>(Movement::GetSignature());
    EcsManager.SetSignature<BoidMovement>(BoidMovement::GetSignature());

    std::default_random_engine random;

    //Add entities
    for (Entity entity = 0; entity < MAXENTITIES; ++entity)
    {
        EcsManager.CreateEntity();

        std::uniform_real_distribution<float> randomPositionX(0.0, SCREEN_WIDTH);
        std::uniform_real_distribution<float> randomPositionY(0.0, SCREEN_HEIGHT);
        std::uniform_real_distribution<float> randomVelocity(-1.0, 1.0);

        EcsManager.AddComponent(entity, Transform {randomPositionX(random), randomPositionY(random)});
        EcsManager.AddComponent(entity, Boid {glm::vec2{randomVelocity(random), randomVelocity(random)}, glm::vec2{0, 0} });
    }

    bool isPaused = false;
    double lastTime = 0.0;
    double lastTitleUpdateTime = 0.0;

    int i = 0;
    while (i < 1000)
    {
        //Calculate delta time
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (!isPaused)
        {
            movementSystem->Update((float)deltaTime, glm::vec2{0, 0});
            boidMovement->Update((float)deltaTime, glm::vec2{0, 0});
        }

        i++;
    }

    return 0;
}*/