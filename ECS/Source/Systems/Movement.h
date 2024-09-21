#pragma once

extern ECSManager EcsManager;

class Movement : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<Transform>());
        signature.set(EcsManager.GetComponentType<Velocity>());
        return signature;
    }

    void Update(float deltaTime, glm::vec2 mousePosition)
    {
        ComponentType transformType = EcsManager.GetComponentType<Transform>();
        ComponentType velocityType = EcsManager.GetComponentType<Velocity>();

        float combinedMass = gravity * attractorMass * particleMass;

        for (const Entity& entity : Entities)
        {
            auto& transform = EcsManager.GetComponent<Transform>(entity, transformType);
            auto& velocity = EcsManager.GetComponent<Velocity>(entity, velocityType);

            glm::vec2 r = mousePosition - transform.Position;
            float rSquared = glm::length(r) + softening;
            glm::vec2 force = (combinedMass * glm::normalize(r) / rSquared);

            glm::vec2 acceleration = force / particleMass;
            transform.Position = transform.Position + (velocity.Value * deltaTime + 0.5f * acceleration * deltaTime * deltaTime);
            velocity.Value = (velocity.Value + acceleration * deltaTime) * damping;
        }
    }

private:
    float damping = 0.999f;
    float attractorMass = 100.0f;
    float particleMass = 10.0f;
    float gravity = 500.0f;
    float softening = 100.0f;
};