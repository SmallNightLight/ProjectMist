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

    void Update(Fixed16_16 deltaTime)
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
        FixedRandom16_16 randomPositionX(Fixed16_16(0), Fixed16_16(SCREEN_WIDTH));
        FixedRandom16_16 randomPositionY(Fixed16_16(0), Fixed16_16(SCREEN_HEIGHT));
        FixedRandom16_16 randomVelocity(Fixed16_16(-1), Fixed16_16(1));
        FixedRandom16_16 randomLifetime(Fixed16_16(2), Fixed16_16(10));

        for(int i = 0; i < destroyedEntities; i++)
        {
            Entity entity = EcsManager.CreateEntity();

            EcsManager.AddComponent(entity, Transform {randomPositionX(random), randomPositionY(random)});
            EcsManager.AddComponent(entity, Velocity {randomVelocity(random), randomVelocity(random)});
            EcsManager.AddComponent(entity, Lifetime {randomLifetime(random)});
        }
    }
};