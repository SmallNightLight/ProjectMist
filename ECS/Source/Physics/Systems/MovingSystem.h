#pragma once

#include "../../ECS/ECS.h"

class MovingSystem final : public System
{
public:
    explicit MovingSystem(PhysicsComponentManager& componentManager) : System()
    {
        colliderTransformCollection = componentManager.GetComponentCollection<ColliderTransform>();
        rigidBodyCollection = componentManager.GetComponentCollection<RigidBodyData>();
        movableCollection = componentManager.GetComponentCollection<Movable>();
    }

    static Signature GetSignature()
    {
        Signature signature;
        signature.set(PhysicsLayer::GetComponentType<ColliderTransform>());
        signature.set(PhysicsLayer::GetComponentType<RigidBodyData>());
        signature.set(PhysicsLayer::GetComponentType<Movable>());
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

private:
    ComponentCollection<ColliderTransform>* colliderTransformCollection;
    ComponentCollection<RigidBodyData>* rigidBodyCollection;
    ComponentCollection<Movable>* movableCollection;
};