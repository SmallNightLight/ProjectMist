#pragma once

#include "../../ECS/ECS.h"

struct Input;

class MovingSystem
{
public:
    using RequiredComponents = ComponentList<Transform, Movable>;

    explicit MovingSystem(PhysicsComponentManager& componentManager)
    {
        transformCollection = componentManager.GetComponentCollection<Transform>();
        movableCollection = componentManager.GetComponentCollection<Movable>();

        Entities.Initialize();
    }

    void Update(Fixed16_16 delta, bool up, bool down, bool left, bool right, bool aPos, bool aNeg)
    {
        for (const Entity& entity : Entities)
        {
            Transform& transform = transformCollection->GetComponent(entity);
            Movable& movable = movableCollection->GetComponent(entity);

            Vector2 velocity = Vector2::Zero();
            Fixed16_16 angularVelocity = Fixed16_16(0);

            if (right)
                velocity.X += 1;

            if (left)
                velocity.X -= 1;

            if (up)
                velocity.Y += 1;

            if (down)
                velocity.Y -= 1;

            if (aPos)
                angularVelocity += 1;

            if (aNeg)
                angularVelocity -= 1;

            velocity = velocity.Normalize();
            Vector2 direction = velocity * (movable.Speed * delta);
            Fixed16_16 rotation = angularVelocity * (movable.Speed * delta) / 10;

            if (direction != Vector2::Zero())
            {
                transform.MovePosition(direction);
            }

            if (rotation != Fixed16_16(0))
            {
                transform.Rotate(rotation);
            }
        }
    }

private:
    ComponentCollection<Transform>* transformCollection;
    ComponentCollection<Movable>* movableCollection;

public:
    EntitySet<MAXENTITIES> Entities;
};