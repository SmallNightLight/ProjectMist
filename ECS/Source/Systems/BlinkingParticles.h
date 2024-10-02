#pragma once

class BlinkingParticles : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<Lifetime>());
        return signature;
    }

    void Update(float deltaTime)
    {
        auto lifeTimeCollection = EcsManager.GetComponentCollection<Lifetime>();

        int destroyedEntities = 0;
        for (const Entity& entity : Entities)
        {
            auto& lifeTime = lifeTimeCollection->GetComponent(entity);

            lifeTime.CurrentLifetime -= deltaTime;

            if (lifeTime.CurrentLifetime <= 0)
            {
                destroyedEntities++;
                EcsManager.MarkEntityForDestruction(entity);
            }
        }

        EcsManager.DestroyMarkedEntities();

        std::default_random_engine random;
        std::uniform_real_distribution<float> randomPositionX(0.0, SCREEN_WIDTH);
        std::uniform_real_distribution<float> randomPositionY(0.0, SCREEN_HEIGHT);
        std::uniform_real_distribution<float> randomVelocity(-1.0, 1.0);
        std::uniform_real_distribution<float> randomLifetime(2.0, 10.0);

        for(int i = 0; i < destroyedEntities; i++)
        {
            Entity entity = EcsManager.CreateEntity();

            EcsManager.AddComponent(entity, Transform {randomPositionX(random), randomPositionY(random)});
            EcsManager.AddComponent(entity, Velocity {randomVelocity(random), randomVelocity(random)});
            EcsManager.AddComponent(entity, Lifetime {randomLifetime(random)});
        }
    }

private:
    float damping = 0.999f;
    float attractorMass = 100.0f;
    float particleMass = 10.0f;
    float gravity = 500.0f;
    float softening = 100.0f;
};