#pragma once

class BlinkingParticles : public System
{
public:
    explicit  BlinkingParticles(Layer* world) : System(world)
    {
        lifeTimeCollection = layer->GetComponentCollection<Lifetime>();
    }

    [[nodiscard]] Signature GetSignature() const override
    {
        Signature signature;
        signature.set(layer->GetComponentType<Lifetime>());
        return signature;
    }

    void Update(Fixed16_16 deltaTime)
    {
        int destroyedEntities = 0;
        for (const Entity& entity : Entities)
        {
            auto& lifeTime = lifeTimeCollection->GetComponent(entity);

            lifeTime.CurrentLifetime -= deltaTime;

            if (lifeTime.CurrentLifetime <= 0)
            {
                destroyedEntities++;
                layer->MarkEntityForDestruction(entity);
            }
        }

        layer->DestroyMarkedEntities();

        std::mt19937 random;
        FixedRandom16_16 randomPositionX(Fixed16_16(0), Fixed16_16(SCREEN_WIDTH));
        FixedRandom16_16 randomPositionY(Fixed16_16(0), Fixed16_16(SCREEN_HEIGHT));
        FixedRandom16_16 randomVelocity(Fixed16_16(-1), Fixed16_16(1));
        FixedRandom16_16 randomLifetime(Fixed16_16(2), Fixed16_16(10));

        for(int i = 0; i < destroyedEntities; i++)
        {
            Entity entity = layer->CreateEntity();

            layer->AddComponent(entity, Transform {randomPositionX(random), randomPositionY(random)});
            layer->AddComponent(entity, Velocity {randomVelocity(random), randomVelocity(random)});
            layer->AddComponent(entity, Lifetime {randomLifetime(random)});
        }
    }

private:
    ComponentCollection<Lifetime>* lifeTimeCollection;
};