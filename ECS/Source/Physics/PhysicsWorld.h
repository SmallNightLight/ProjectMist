#pragma once
#include "PhysicsSettings.h"

class PhysicsWorld : public ECSWorld
{
public:
    PhysicsWorld()
    {
        Setup();

        //Register components
        RegisterComponent<ColliderTransform>();
        RegisterComponent<RigidBodyData>();
        RegisterComponent<Movable>();
        RegisterComponent<ColliderRenderData>();
        RegisterComponent<BoxCollider>();
        RegisterComponent<CircleCollider>();
        RegisterComponent<Camera>();

        //Register systems
        rigidBodySystem = RegisterSystem<RigidBody>();
        movingSystem = RegisterSystem<MovingSystem>();
        boxColliderRenderer = RegisterSystem<BoxColliderRenderer>();
        circleColliderRenderer = RegisterSystem<CircleColliderRenderer>();
        cameraSystem = RegisterSystem<CameraSystem>();

        //Set signatures
        SetSignature<RigidBody>(rigidBodySystem->GetSignature());
        SetSignature<MovingSystem>(movingSystem->GetSignature());
        SetSignature<BoxColliderRenderer>(boxColliderRenderer->GetSignature());
        SetSignature<CircleColliderRenderer>(circleColliderRenderer->GetSignature());
        SetSignature<CameraSystem>(cameraSystem->GetSignature());

        //Add camera
        Entity cameraEntity = CreateEntity();
        camera = AddComponent(cameraEntity, Camera(static_cast<Fixed16_16>(SCREEN_WIDTH), static_cast<Fixed16_16>(SCREEN_HEIGHT), Fixed16_16(20)));

        //Add a ground
        CreateBox(Vector2(Fixed16_16(0), camera->Bottom), camera->Right - camera->Left + Fixed16_16(10), Fixed16_16(1), Kinematic);

        //Create rotated objects
        Entity e1 = CreateBox(Vector2(100, 0), Fixed16_16(30), Fixed16_16(1), Kinematic);
        GetComponent<ColliderTransform>(e1).Rotate(Fixed16_16(0, 1));

        Entity e2 = CreateBox(Vector2(-95, -10), Fixed16_16(30), Fixed16_16(1), Kinematic);
        GetComponent<ColliderTransform>(e2).Rotate(Fixed16_16(0, -1));
    }

    Entity CreateCircle(const Vector2& position, const Fixed16_16& radius, RigidBodyType shape = Dynamic, float r = 1.0f, float g = 1.0f, float b = 1.0f)
    {
        Entity entity = CreateEntity();

        AddComponent(entity, ColliderTransform(position, Fixed16_16(0), Circle, shape));
        AddComponent(entity, CircleCollider(radius));
        AddComponent(entity, RigidBodyData::CreateCircleRigidBody(radius, Fixed16_16(1), Fixed16_16(0, 5), Fixed16_16(0, 6), Fixed16_16(0, 4)));
        AddComponent(entity, ColliderRenderData(r, g, b));

        return entity;
    }

    Entity CreateRandomCircle()
    {
        FixedRandom16_16 randomPositionX(camera->Left, camera->Right);
        FixedRandom16_16 randomPositionY(camera->Bottom, camera->Top);
        std::uniform_real_distribution<float> randomColor(0.0, 1.0);

        return CreateCircle(Vector2(randomPositionX(random), randomPositionY(random)), Fixed16_16(1), Dynamic, randomColor(random),randomColor(random), randomColor(random));
    }

    Entity CreateBox(const Vector2& position, const Fixed16_16& width, const Fixed16_16& height, RigidBodyType shape = Dynamic, float r = 1.0f, float g = 1.0f, float b = 1.0f)
    {
        Entity entity = CreateEntity();

        AddComponent(entity, ColliderTransform(position, Fixed16_16(0), Box, shape));
        AddComponent(entity, BoxCollider(width, height));
        AddComponent(entity, RigidBodyData::CreateBoxRigidBody(width, height, Fixed16_16(1), Fixed16_16(0, 5), Fixed16_16(0, 6), Fixed16_16(0, 4)));
        AddComponent(entity, ColliderRenderData(r, g, b));

        return entity;
    }

    Entity CreateRandomBox()
    {
        FixedRandom16_16 randomPositionX(camera->Left, camera->Right);
        FixedRandom16_16 randomPositionY(camera->Bottom, camera->Top);
        std::uniform_real_distribution<float> randomColor(0.0, 1.0);

        return CreateBox(Vector2(randomPositionX(random), randomPositionY(random)), Fixed16_16(2), Fixed16_16(2), Dynamic, randomColor(random),randomColor(random), randomColor(random));
    }

    //Entity AddConvexObject()
    //{
    //
    //}

    void Update(GLFWwindow* window, Fixed16_16 deltaTime)
    {
        UpdateDebug(window);

        Fixed16_16 stepTime = deltaTime / Iterations;

        for(int i = 0; i < Iterations; ++i)
        {
            movingSystem->Update(window, stepTime);

            rigidBodySystem->ApplyVelocity(stepTime);
            rigidBodySystem->DetectCollisions();

            rigidBodySystem->WrapEntities(*camera);

        }
    }

    void Render()
    {
        cameraSystem->Apply();
        boxColliderRenderer->Render();
        circleColliderRenderer->Render();

        //Debug
        if (RenderCollisionPoints)
        {
            CircleColliderRenderer::RenderContactPoints(rigidBodySystem->collisionsRE);
        }

        if (RenderBoundingBoxes)
        {
            circleColliderRenderer->RenderAABB();
            boxColliderRenderer->RenderAABB();
        }
    }

    //Debug
    static void SetupDebug(GLFWwindow* window)
    {
        glfwSetMouseButtonCallback(window, DebugCallbackMouse);
    }

    void UpdateDebug(GLFWwindow* window)
    {
        if (createBox)
        {
            CreateBox(GetMousePosition(window), Fixed16_16(2), Fixed16_16(2), Dynamic, 0.5f, 0.5f, 0.5f);
            createBox = false;
        }
        if (createCircle)
        {
            CreateCircle(GetMousePosition(window), Fixed16_16(1), Dynamic, 0.5f, 0.5f, 0.5f);
            createCircle = false;
        }
    }

    static void DebugCallbackMouse(GLFWwindow* window, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            createBox = true;
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            createCircle = true;
        }
    }

private:
    Vector2 GetMousePosition(GLFWwindow* window)
    {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        Fixed16_16 normalizedX = Fixed16_16::FromFloat<double>(mouseX) / Fixed16_16::FromFloat<double>(SCREEN_WIDTH);
        Fixed16_16 normalizedY = Fixed16_16::FromFloat<double>(SCREEN_HEIGHT - mouseY) / Fixed16_16::FromFloat<double>(SCREEN_HEIGHT);

        Fixed16_16 worldX = camera->Left + normalizedX * (camera->Right - camera->Left);
        Fixed16_16 worldY = camera->Bottom + normalizedY * (camera->Top - camera->Bottom);

        return Vector2 { worldX, worldY };
    }

private:
    std::shared_ptr<RigidBody> rigidBodySystem;
    std::shared_ptr<MovingSystem> movingSystem;
    std::shared_ptr<BoxColliderRenderer> boxColliderRenderer;
    std::shared_ptr<CircleColliderRenderer> circleColliderRenderer;
    std::shared_ptr<CameraSystem> cameraSystem;

    std::default_random_engine random;
    static inline bool createBox = false;
    static inline bool createCircle = false;

    Camera* camera;
};
