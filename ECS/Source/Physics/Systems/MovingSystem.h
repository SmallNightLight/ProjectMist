#pragma once

class MovingSystem : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<Movable>());
        signature.set(EcsManager.GetComponentType<ColliderTransform>());
        return signature;
    }

    void Update(GLFWwindow* window, Fixed16_16 delta)
    {
        auto colliderTransformCollection = EcsManager.GetComponentCollection<ColliderTransform>();
        auto movableCollection = EcsManager.GetComponentCollection<Movable>();

        for (const Entity& entity : Entities)
        {
            ColliderTransform& colliderTransform = colliderTransformCollection->GetComponent(entity);
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
            Vector2 direction = input * movable.Speed * delta;

            if (direction != Vector2::Zero())
            {
                colliderTransform.MovePosition(direction);
                if (colliderTransform.Shape == Box)
                {
                    EcsManager.GetComponent<BoxCollider>(entity).TransformUpdateRequired = true;
                }
            }

        }
    }
};