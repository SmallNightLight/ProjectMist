#pragma once

#include "../../ECS/ECS.h"

class MovingSystem : public System
{
public:
    explicit MovingSystem(Layer* world) : System(world)
    {
        colliderTransformCollection = layer->GetComponentCollection<ColliderTransform>();
        rigidBodyCollection = layer->GetComponentCollection<RigidBodyData>();
        movableCollection = layer->GetComponentCollection<Movable>();
    }

    [[nodiscard]] Signature GetSignature() const override
    {
        Signature signature;
        signature.set(layer->GetComponentType<ColliderTransform>());
        signature.set(layer->GetComponentType<RigidBodyData>());
        signature.set(layer->GetComponentType<Movable>());
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
    std::shared_ptr<ComponentCollection<ColliderTransform>> colliderTransformCollection;
    std::shared_ptr<ComponentCollection<RigidBodyData>> rigidBodyCollection;
    std::shared_ptr<ComponentCollection<Movable>> movableCollection;
};