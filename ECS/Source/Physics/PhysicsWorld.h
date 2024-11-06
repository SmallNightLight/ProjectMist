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
        RegisterComponent<CircleCollider>();
        RegisterComponent<BoxCollider>();
        RegisterComponent<PolygonCollider>();
        RegisterComponent<Camera>();

        //Register systems
        rigidBodySystem = RegisterSystem<RigidBody>();
        movingSystem = RegisterSystem<MovingSystem>();
        circleColliderRenderer = RegisterSystem<CircleColliderRenderer>();
        boxColliderRenderer = RegisterSystem<BoxColliderRenderer>();
        polygonColliderRenderer = RegisterSystem<PolygonColliderRenderer>();
        cameraSystem = RegisterSystem<CameraSystem>();

        //Set signatures
        SetSignature<RigidBody>(rigidBodySystem->GetSignature());
        SetSignature<MovingSystem>(movingSystem->GetSignature());
        SetSignature<CircleColliderRenderer>(circleColliderRenderer->GetSignature());
        SetSignature<BoxColliderRenderer>(boxColliderRenderer->GetSignature());
        SetSignature<PolygonColliderRenderer>(polygonColliderRenderer->GetSignature());
        SetSignature<CameraSystem>(cameraSystem->GetSignature());

        //Set seed for deterministic number generation
        numberGenerator = std::mt19937(12);

        //Add camera
        Entity cameraEntity = CreateEntity();
        camera = AddComponent(cameraEntity, Camera(static_cast<Fixed16_16>(SCREEN_WIDTH), static_cast<Fixed16_16>(SCREEN_HEIGHT), Fixed16_16(20)));

        //Add a ground
        CreateBox(Vector2(Fixed16_16(0), camera->Bottom), camera->Right - camera->Left + Fixed16_16(10), Fixed16_16(2), Static);

        //Add walls
        CreateBox(Vector2(camera->Left, Fixed16_16(0)), Fixed16_16(2), Fixed16_16(50), Static);
        CreateBox(Vector2(camera->Right, Fixed16_16(0)), Fixed16_16(2), Fixed16_16(50), Static);

        //Create rotated objects
        Entity e1 = CreateBox(Vector2(10, 0), Fixed16_16(25), Fixed16_16(1), Static);
        GetComponent<ColliderTransform>(e1).Rotate(Fixed16_16(0, 1));

        Entity e2 = CreateBox(Vector2(-10, -10), Fixed16_16(25), Fixed16_16(1), Static);
        GetComponent<ColliderTransform>(e2).Rotate(Fixed16_16(0, -1));
    }

    Entity CreateCircle(const Vector2& position, const Fixed16_16& radius, RigidBodyType shape = Dynamic, float r = 1.0f, float g = 1.0f, float b = 1.0f)
    {
        Entity entity = CreateEntity();

        AddComponent(entity, ColliderTransform(position, Fixed16_16(0), Circle, shape));
        AddComponent(entity, CircleCollider(radius));
        AddComponent(entity, RigidBodyData::CreateCircleRigidBody(radius, Fixed16_16(1), Fixed16_16(0, 5), Fixed16_16(0, 8), Fixed16_16(0, 4)));
        AddComponent(entity, ColliderRenderData(r, g, b));

        return entity;
    }

    Entity CreateRandomCircle()
    {
        FixedRandom16_16 randomPositionX(camera->Left, camera->Right);
        FixedRandom16_16 randomPositionY(camera->Bottom, camera->Top);
        std::uniform_real_distribution<float> randomColor(0.0, 1.0);

        return CreateCircle(Vector2(randomPositionX(numberGenerator), randomPositionY(numberGenerator)), Fixed16_16(1), Dynamic, randomColor(numberGenerator), randomColor(numberGenerator), randomColor(numberGenerator));
    }

    Entity CreateBox(const Vector2& position, const Fixed16_16& width, const Fixed16_16& height, RigidBodyType shape = Dynamic, float r = 1.0f, float g = 1.0f, float b = 1.0f)
    {
        Entity entity = CreateEntity();

        AddComponent(entity, ColliderTransform(position, Fixed16_16(0), Box, shape));
        AddComponent(entity, BoxCollider(width, height));
        AddComponent(entity, RigidBodyData::CreateBoxRigidBody(width, height, Fixed16_16(1), Fixed16_16(0, 5), Fixed16_16(0, 8), Fixed16_16(0, 4)));
        AddComponent(entity, ColliderRenderData(r, g, b));

        return entity;
    }

    Entity CreateRandomBox() //TODO: Make all of these constexpr
    {
        FixedRandom16_16 randomPositionX(camera->Left, camera->Right);
        FixedRandom16_16 randomPositionY(camera->Bottom, camera->Top);
        std::uniform_real_distribution<float> randomColor(0.0, 1.0);

        return CreateBox(Vector2(randomPositionX(numberGenerator), randomPositionY(numberGenerator)), Fixed16_16(2), Fixed16_16(2), Dynamic, randomColor(numberGenerator), randomColor(numberGenerator), randomColor(numberGenerator));
    }

    Entity CreatePolygon(const Vector2& position, const std::vector<Vector2>& vertices, RigidBodyType shape = Dynamic, float r = 1.0f, float g = 1.0f, float b = 1.0f)
    {
        Entity entity = CreateEntity();

        AddComponent(entity, ColliderTransform(position, Fixed16_16(0), Polygon, shape));
        AddComponent(entity, PolygonCollider(vertices));
        AddComponent(entity, RigidBodyData::CreatePolygonRigidBody(vertices, Fixed16_16(1), Fixed16_16(0, 5), Fixed16_16(0, 8), Fixed16_16(0, 4)));
        AddComponent(entity, ColliderRenderData(r, g, b));

        return entity;
    }

    Entity CreateRandomPolygon()
    {
        int numSides = 5;

        FixedRandom16_16 randomPositionX(camera->Left, camera->Right);
        FixedRandom16_16 randomPositionY(camera->Bottom, camera->Top);
        std::uniform_real_distribution<float> randomColor(0.0, 1.0);

        // Generate a random center point for the polygon
        Vector2 center(randomPositionX(numberGenerator), randomPositionY(numberGenerator));

        // Random radius for the polygon
        FixedRandom16_16 randomRadius(Fixed16_16(1), Fixed16_16(5)); // Choose an appropriate range for radius
        Fixed16_16 radius = randomRadius(numberGenerator);

        // Generate the vertices by evenly spacing them around the circle
        std::vector<Vector2> vertices;
        Fixed16_16 angleIncrement = Fixed16_16(2) * Fixed16_16::pi() / Fixed16_16(numSides);

        for (int i = 0; i < numSides; ++i)
        {
            Fixed16_16 angle = angleIncrement * Fixed16_16(i);
            Vector2 vertex(center.X + radius * cos(angle), center.Y + radius * sin(angle));
            vertices.push_back(vertex);
        }

        // Return the polygon with the generated vertices
        return CreatePolygon(Vector2(randomPositionX(numberGenerator), randomPositionY(numberGenerator)), vertices, Dynamic, randomColor(numberGenerator), randomColor(numberGenerator), randomColor(numberGenerator));
    }


    void Update(GLFWwindow* window, Fixed16_16 deltaTime)
    {
        UpdateDebug(window);

        Fixed16_16 stepTime = deltaTime / Iterations;

        for(int i = 0; i < Iterations; ++i)
        {
            movingSystem->Update(window, stepTime);

            rigidBodySystem->ApplyVelocity(stepTime);
            rigidBodySystem->DetectCollisions();

            //rigidBodySystem->WrapEntities(*camera);

        }
    }

    void Render()
    {
        cameraSystem->Apply();

        circleColliderRenderer->Render();
        boxColliderRenderer->Render();
        polygonColliderRenderer->Render();

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
    std::shared_ptr<CircleColliderRenderer> circleColliderRenderer;
    std::shared_ptr<BoxColliderRenderer> boxColliderRenderer;
    std::shared_ptr<PolygonColliderRenderer> polygonColliderRenderer;
    std::shared_ptr<CameraSystem> cameraSystem;

    std::mt19937 numberGenerator;
    static inline bool createBox = false;
    static inline bool createCircle = false;

    Camera* camera;
};
