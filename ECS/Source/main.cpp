#include "Base/ECSManager.h"
#include "Components/ComponentHeaders.h"
#include "Systems/SystemHeader.h"

#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

ECSManager EcsManager;

void SetFrameSize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

glm::vec2 GetMousePosition(GLFWwindow* window)
{
    //Get the mouse position in pixel coordinates
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    //Convert mouse X and Y to normalized screen coordinates [-1, 1]
    return glm::vec2 {(float)mouseX, SCREEN_HEIGHT - (float)mouseY };
}

int main()
{
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

        std::uniform_real_distribution<float> randomPositionX(0.0f, SCREEN_WIDTH);
        std::uniform_real_distribution<float> randomPositionY(0.0f, SCREEN_HEIGHT);
        std::uniform_real_distribution<float> randomSize(1.0f, 30.0f);
        std::uniform_int_distribution<int> randomColor(0, 255);

        //Particles with split components
        EcsManager.AddComponent(entity, Transform {randomPositionX(random), randomPositionY(random)});
        EcsManager.AddComponent(entity, Velocity {0, 0});
        //EcsManager.AddComponent(entity, RectangleData{ randomSize(random),randomSize(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random)});


        //Particles with one component and one system that handles movement and rendering
        //EcsManager.AddComponent(entity, MovingParticleData {randomPosition(random), randomPosition(random), 0, 0, randomSize(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random), (std::uint8_t)randomColor(random)});
    }


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
        movementSystem->Update((float)deltaTime, GetMousePosition(window));
        particleRenderer->Render();
        rectangleRenderer->Render();
        movingParticleRenderer->Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //Stop glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}