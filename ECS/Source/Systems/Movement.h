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

    void Update()
    {
        ComponentType transformType = EcsManager.GetComponentType<Transform>();
        ComponentType velocityType = EcsManager.GetComponentType<Velocity>();

        for (const Entity& entity : Entities)
        {
            auto& transform = EcsManager.GetComponent<Transform>(entity, transformType);
            auto& velocity = EcsManager.GetComponent<Velocity>(entity, velocityType);

            velocity.X = 0;
            velocity.Y = -0.02f;

            transform.X += velocity.X;
            transform.Y += velocity.Y;

            if (transform.Y < 0)
            {
                transform.Y = 700;
            }
        }
    }
};