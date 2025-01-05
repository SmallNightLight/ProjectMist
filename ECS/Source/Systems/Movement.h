#pragma once

class Movement : public System
{
public:
    explicit  Movement(Layer* world) : System(world)
    {
        transformCollection = layer->GetComponentCollection<Transform>();
        velocityCollection = layer->GetComponentCollection<Velocity>();
    }

    [[nodiscard]] Signature GetSignature() const override
    {
        Signature signature;
        signature.set(layer->GetComponentType<Transform>());
        signature.set(layer->GetComponentType<Velocity>());
        return signature;
    }

    void Update(Fixed16_16 deltaTime, Vector2 mousePosition)
    {
        Fixed16_16 combinedMass = gravity * attractorMass * particleMass;

        for (const Entity& entity : Entities)
        {
            auto& transform = transformCollection->GetComponent(entity);
            auto& velocity = velocityCollection->GetComponent(entity);

            Vector2 r = mousePosition - transform.Position;
            Fixed16_16 rSquared = r.Magnitude() * softening;
            Vector2 force = (r.Normalize() * combinedMass  / rSquared);
            Vector2 acceleration = force / particleMass;

            Vector2 newVelocity = velocity.Value + acceleration * deltaTime;
            transform.Position += (velocity.Value + newVelocity) * Fixed16_16(0, 5) * deltaTime;
            velocity.Value = newVelocity;
        }
    }

private:
    Fixed16_16 damping = Fixed16_16::FromFixed(0, 999);
    Fixed16_16 attractorMass = Fixed16_16::FromFixed(100, 0);
    Fixed16_16 particleMass = Fixed16_16::FromFixed(0, 1);
    Fixed16_16 gravity = Fixed16_16::FromFixed(2000, 0);
    Fixed16_16 softening = Fixed16_16::FromFixed(0, 5);

    std::shared_ptr<ComponentCollection<Transform>> transformCollection;
    std::shared_ptr<ComponentCollection<Velocity>> velocityCollection;
};