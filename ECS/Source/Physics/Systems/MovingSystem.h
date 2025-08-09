#pragma once

#include "../../ECS/ECS.h"

class MovingSystem
{
public:
    explicit MovingSystem(PhysicsComponentManager& componentManager)
    {
        colliderTransformCollection = componentManager.GetComponentCollection<ColliderTransform>();
        rigidBodyCollection = componentManager.GetComponentCollection<RigidBodyData>();
        movableCollection = componentManager.GetComponentCollection<Movable>();

        Entities.Initialize();
    }

    static constexpr PhysicsSignature GetSignature()
    {
        PhysicsSignature signature;
        signature.set(PhysicsComponentManager::GetComponentType<ColliderTransform>());
        signature.set(PhysicsComponentManager::GetComponentType<RigidBodyData>());
        signature.set(PhysicsComponentManager::GetComponentType<Movable>());
        return signature;
    }

    void Update(GLFWwindow* window, Fixed16_16 delta)
    {
        for (const Entity& entity : Entities)
        {
            ColliderTransform& colliderTransform = colliderTransformCollection->GetComponent(entity);
            RigidBodyData& rigidBodyData = rigidBodyCollection->GetComponent(entity);
            Movable& movable = movableCollection->GetComponent(entity);

            Vector2 input = Vector2::Zero();

            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                input.X += 1;

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                input.X -= 1;

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                input.Y += 1;

            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                input.Y -= 1;

            input = input.Normalize();
            Vector2 direction = input * movable.Speed;

            if (direction != Vector2::Zero())
            {
                rigidBodyData.ApplyForce(direction);
            }

        }
    }

public:
    EntitySet<MAXENTITIES> Entities;

private:
    ComponentCollection<ColliderTransform>* colliderTransformCollection;
    ComponentCollection<RigidBodyData>* rigidBodyCollection;
    ComponentCollection<Movable>* movableCollection;
};