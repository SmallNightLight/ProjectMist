#pragma once

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
        cameraEntity = CreateEntity();
        AddComponent(cameraEntity, Camera(static_cast<Fixed16_16>(SCREEN_WIDTH), static_cast<Fixed16_16>(SCREEN_HEIGHT), Fixed16_16(20)));
    }

    Entity CreateCircle(const Vector2& position, const Fixed16_16& radius, float r = 1.0f, float g = 1.0f, float b = 1.0f)
    {
        Entity entity = CreateEntity();

        AddComponent(entity, ColliderTransform(position, Fixed16_16(0), Circle, Static));
        AddComponent(entity, CircleCollider(radius));
        AddComponent(entity, RigidBodyData());
        AddComponent(entity, ColliderRenderData(r, g, b));

        return entity;
    }

    Entity CreateRandomCircle()
    {
        Camera camera = GetComponent<Camera>(cameraEntity);
        FixedRandom16_16 randomPositionX(camera.Left, camera.Right);
        FixedRandom16_16 randomPositionY(camera.Bottom, camera.Top);
        std::uniform_real_distribution<float> randomColor(0.0, 1.0);

        return CreateCircle(Vector2(randomPositionX(random), randomPositionY(random)), Fixed16_16(1), randomColor(random),randomColor(random), randomColor(random));
    }

    Entity CreateBox(const Vector2& position, const Fixed16_16& width, const Fixed16_16& height, float r = 1.0f, float g = 1.0f, float b = 1.0f)
    {
        Entity entity = CreateEntity();

        AddComponent(entity, ColliderTransform(position, Fixed16_16(0), Box, Static));
        AddComponent(entity, BoxCollider(width, height));
        AddComponent(entity, RigidBodyData());
        AddComponent(entity, ColliderRenderData(r, g, b));

        return entity;
    }

    Entity CreateRandomBox()
    {
        Camera camera = GetComponent<Camera>(cameraEntity);
        FixedRandom16_16 randomPositionX(camera.Left, camera.Right);
        FixedRandom16_16 randomPositionY(camera.Bottom, camera.Top);
        std::uniform_real_distribution<float> randomColor(0.0, 1.0);

        return CreateBox(Vector2(randomPositionX(random), randomPositionY(random)), Fixed16_16(2), Fixed16_16(2), randomColor(random),randomColor(random), randomColor(random));
    }

    //Entity AddConvexObject()
    //{
    //
    //}

    void Update(GLFWwindow* window, Fixed16_16 deltaTime)
    {
        movingSystem->Update(window, deltaTime);

        rigidBodySystem->ApplyVelocity(deltaTime);
        rigidBodySystem->DetectCollisions();
        rigidBodySystem->RotateAllEntities(deltaTime);

        rigidBodySystem->WrapEntities(GetComponent<Camera>(cameraEntity));
    }

    void Render()
    {
        cameraSystem->Apply();
        boxColliderRenderer->Render();
        circleColliderRenderer->Render();
    }

private:
    std::shared_ptr<RigidBody> rigidBodySystem;
    std::shared_ptr<MovingSystem> movingSystem;
    std::shared_ptr<BoxColliderRenderer> boxColliderRenderer;
    std::shared_ptr<CircleColliderRenderer> circleColliderRenderer;
    std::shared_ptr<CameraSystem> cameraSystem;

    std::default_random_engine random;

    Entity cameraEntity;
};