#include "Base/ECSWorld.h"
#include "Components/ComponentHeaders.h"
#include "Systems/SystemHeader.h"

#include <iostream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Math/TestMath.h"
#include "Physics/Physics.h"

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
    ECSWorld EcsWorld;
    EcsWorld.Setup();

    //Register components
    EcsWorld.RegisterComponent<Transform>();
    EcsWorld.RegisterComponent<Velocity>();
    EcsWorld.RegisterComponent<Lifetime>();
    EcsWorld.RegisterComponent<Boid>();
    EcsWorld.RegisterComponent<Camera>();

    EcsWorld.RegisterComponent<ColliderTransform>();
    EcsWorld.RegisterComponent<RigidBodyData>();
    EcsWorld.RegisterComponent<Movable>();
    EcsWorld.RegisterComponent<ColliderRenderData>();
    EcsWorld.RegisterComponent<BoxCollider>();
    EcsWorld.RegisterComponent<CircleCollider>();

    //Register systems
    auto movementSystem = EcsWorld.RegisterSystem<Movement>();
    auto particleRenderer = EcsWorld.RegisterSystem<ParticleRenderer>();
    auto blinkingParticles = EcsWorld.RegisterSystem<BlinkingParticles>();
    auto boidMovement = EcsWorld.RegisterSystem<BoidMovement>();
    auto boidRenderer = EcsWorld.RegisterSystem<BoidRenderer>();
    auto cameraSystem = EcsWorld.RegisterSystem<CameraSystem>();

    auto rigidBodySystem = EcsWorld.RegisterSystem<RigidBody>();
    auto movingSystem = EcsWorld.RegisterSystem<MovingSystem>();

    auto boxColliderRenderer = EcsWorld.RegisterSystem<BoxColliderRenderer>();
    auto circleColliderRenderer = EcsWorld.RegisterSystem<CircleColliderRenderer>();

    //Setup signatures
    EcsWorld.SetSignature<Movement>(movementSystem->GetSignature());
    EcsWorld.SetSignature<ParticleRenderer>(particleRenderer->GetSignature());
    EcsWorld.SetSignature<BlinkingParticles>(blinkingParticles->GetSignature());
    EcsWorld.SetSignature<BoidMovement>(boidMovement->GetSignature());
    EcsWorld.SetSignature<BoidRenderer>(boidRenderer->GetSignature());
    EcsWorld.SetSignature<CameraSystem>(cameraSystem->GetSignature());

    EcsWorld.SetSignature<RigidBody>(rigidBodySystem->GetSignature());
    EcsWorld.SetSignature<MovingSystem>(movingSystem->GetSignature());

    EcsWorld.SetSignature<BoxColliderRenderer>(boxColliderRenderer->GetSignature());
    EcsWorld.SetSignature<CircleColliderRenderer>(circleColliderRenderer->GetSignature());


    //Add camera
    Entity cameraEntity = EcsWorld.CreateEntity();
    EcsWorld.AddComponent(cameraEntity, Camera(static_cast<Fixed16_16>(SCREEN_WIDTH), static_cast<Fixed16_16>(SCREEN_HEIGHT), Fixed16_16(20)));


    //Add physics objects
    Camera camera = EcsWorld.GetComponent<Camera>(cameraEntity);
    std::default_random_engine random;
    FixedRandom16_16 randomPositionX(camera.Left, camera.Right);
    FixedRandom16_16 randomPositionY(camera.Bottom, camera.Top);
    FixedRandom16_16 randomVelocity(Fixed16_16(-1), Fixed16_16(1));
    FixedRandom16_16 randomLifetime(Fixed16_16(0), Fixed16_16(3));
    std::uniform_real_distribution<float> randomColor(0.0, 1.0);

    //Add circles
    for (int i = 0; i < 20; ++i)
    {
        Entity entity = EcsWorld.CreateEntity();

        EcsWorld.AddComponent(entity, ColliderTransform(Vector2(randomPositionX(random), randomPositionY(random)), Fixed16_16(0), ColliderType::Circle, RigidBodyType::Static));
        EcsWorld.AddComponent(entity, CircleCollider(Fixed16_16(1)));
        EcsWorld.AddComponent(entity, RigidBodyData());
        EcsWorld.AddComponent(entity, ColliderRenderData(randomColor(random),randomColor(random), randomColor(random)));
    }

    EcsWorld.AddComponent(10, Movable(Fixed16_16(0, 1)));

    //Add boxes
    for (int i = 0; i < 20; ++i)
    {
        Entity entity = EcsWorld.CreateEntity();

        EcsWorld.AddComponent(entity, ColliderTransform(Vector2(randomPositionX(random), randomPositionY(random)), Fixed16_16(0), ColliderType::Box, RigidBodyType::Static));
        EcsWorld.AddComponent(entity, BoxCollider(Fixed16_16(2), Fixed16_16(2)));
        EcsWorld.AddComponent(entity, RigidBodyData());
        EcsWorld.AddComponent(entity, ColliderRenderData(randomColor(random),randomColor(random), randomColor(random)));
    }

    /*for (Entity entity = 0; entity < 20; ++entity)
    {
        EcsWorld.CreateEntity();

        EcsWorld.AddComponent(entity, ColliderTransform(Vector2(randomPositionX(random), randomPositionY(random)), Fixed16_16(0), ColliderType::Circle, RigidBodyType::Static));
        EcsWorld.AddComponent(entity, CircleCollider(Fixed16_16(30)));
        //EcsWorld.AddComponent(entity, BoxCollider(Fixed16_16(100), Fixed16_16(100)));
        EcsWorld.AddComponent(entity, ColliderRenderData(0.5, 0.5, 0.5));

        //EcsWorld.AddComponent(entity, Transform {randomPositionX(random), randomPositionY(random)});
        //EcsWorld.AddComponent(entity, Velocity {randomVelocity(random), randomVelocity(random)});
        //EcsWorld.AddComponent(entity, Lifetime {randomLifetime(random)});
        //EcsWorld.AddComponent(entity, Boid {Vector2{randomVelocity(random), randomVelocity(random)}, Vector2{0, 0} });
    }*/

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

        Fixed16_16 fixedDelta = Fixed16_16(1) / Fixed16_16(60);
        if (!isPaused)
        {
            movementSystem->Update(Fixed16_16::FromFloat(deltaTime), GetMousePosition(window));
            boidMovement->Update(Fixed16_16::FromFloat(deltaTime));
            blinkingParticles->Update(Fixed16_16::FromFloat(deltaTime));

            movingSystem->Update(window, fixedDelta);
            rigidBodySystem->Update();
            rigidBodySystem->RotateAllEntities(fixedDelta);
        }

        //Rendering
        cameraSystem->Apply();
        particleRenderer->Render();
        boidRenderer->Render();

        boxColliderRenderer->Render();
        circleColliderRenderer->Render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //Stop glfw
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}