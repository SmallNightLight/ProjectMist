

#include "Game/Game.h"


int main()
{
    Game game = Game();
    return game.GameLoop();
}

//OLD CODE
 //return TestMath::Test();



    //ECS setup
    //OldPhysicsWorld physicsWorld = OldPhysicsWorld();

    //Layer EcsWorld = Layer();

    /*//Register components
    EcsWorld.RegisterComponent<Transform>();
    EcsWorld.RegisterComponent<Velocity>();
    EcsWorld.RegisterComponent<Lifetime>();
    EcsWorld.RegisterComponent<Boid>();
    EcsWorld.RegisterComponent<Camera>();


    //Register systems
    auto movementSystem = EcsWorld.RegisterSystem<Movement>();
    auto particleRenderer = EcsWorld.RegisterSystem<ParticleRenderer>();
    auto blinkingParticles = EcsWorld.RegisterSystem<BlinkingParticles>();
    auto boidMovement = EcsWorld.RegisterSystem<BoidMovement>();
    auto boidRenderer = EcsWorld.RegisterSystem<BoidRenderer>();
    auto cameraSystem = EcsWorld.RegisterSystem<CameraSystem>();

    //Add camera
    //Entity cameraEntity = EcsWorld.CreateEntity();
    //EcsWorld.AddComponent(cameraEntity, Camera(static_cast<Fixed16_16>(SCREEN_WIDTH), static_cast<Fixed16_16>(SCREEN_HEIGHT), Fixed16_16(20)));


    //Add physics objects
    //Add circles*/
    /*for (int i = 0; i < 15; ++i)
    {
        physicsWorld.CreateRandomCircle();
    }

    physicsWorld.AddComponent(10, Movable(Fixed16_16(20)));

    //Add boxes/
    for (int i = 0; i < 15; ++i)
    {
        physicsWorld.CreateRandomBox();
    }

    for (int i = 0; i < 15; ++i)
    {
        physicsWorld.CreateRandomPolygon();
    }*/

    /*for (Entity entity = 0; entity < 20; ++entity)
    {
        EcsWorld.CreateEntity();

        EcsWorld.AddComponent(entity, ColliderTransform(Vector2(randomPositionX(numberGenerator), randomPositionY(numberGenerator)), Fixed16_16(0), ColliderType::Circle, RigidBodyType::Static));
        EcsWorld.AddComponent(entity, CircleCollider(Fixed16_16(30)));
        //EcsWorld.AddComponent(entity, BoxCollider(Fixed16_16(100), Fixed16_16(100)));
        EcsWorld.AddComponent(entity, ColliderRenderData(0.5, 0.5, 0.5));

        //EcsWorld.AddComponent(entity, Transform {randomPositionX(numberGenerator), randomPositionY(numberGenerator)});
        //EcsWorld.AddComponent(entity, Velocity {randomVelocity(numberGenerator), randomVelocity(numberGenerator)});
        //EcsWorld.AddComponent(entity, Lifetime {randomLifetime(numberGenerator)});
        //EcsWorld.AddComponent(entity, Boid {Vector2{randomVelocity(numberGenerator), randomVelocity(numberGenerator)}, Vector2{0, 0} });
    }*/

    //bool isPaused = false;
    //double lastTime = 0.0;
    //double lastTitleUpdateTime = 0.0;

    //OldPhysicsWorld::SetupDebug(window);
