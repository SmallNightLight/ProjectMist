#pragma once

class RigidBody : public System
{
public:
    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<ColliderTransform>());
        signature.set(EcsManager.GetComponentType<RigidBodyData>());
        return signature;
    }

    void Update()
    {
        auto colliderTransformCollection = EcsManager.GetComponentCollection<ColliderTransform>();

        CollisionDetection collisionDetection = CollisionDetection();
        CollisionInfo collisionInfo(Vector2::Zero(), 0, 0, Fixed16_16(0));

        for (auto it1 = Entities.begin(); it1 != Entities.end(); ++it1)
        {
            const Entity& entity1 = *it1;
            ColliderTransform& colliderTransform1 = colliderTransformCollection->GetComponent(entity1);

            //Detect collisions
            for (auto it2 = std::next(it1); it2 != Entities.end(); ++it2)
            {
                const Entity& entity2 = *it2;
                ColliderTransform& colliderTransform2 = colliderTransformCollection->GetComponent(entity2);

                //Check for different shape types and do the correct collision detection
                if (colliderTransform1.Shape == Circle)
                {
                    if (colliderTransform2.Shape == Circle)
                    {
                        if (collisionDetection.CircleCircleCollision(entity1, entity2, colliderTransform1, colliderTransform2, collisionInfo))
                        {
                            colliderTransform1.MovePosition(collisionInfo.Normal  * collisionInfo.Depth / 2);
                            colliderTransform2.MovePosition(collisionInfo.Normal * - 1* collisionInfo.Depth / 2);
                        }
                    }
                    else if (colliderTransform2.Shape == Box)
                    {
                        std::cout << "Collision type not defined" << std::endl;
                    }
                    else if (colliderTransform2.Shape == Convex)
                    {
                        std::cout << "Collision type not defined" << std::endl;
                    }
                }
                else if (colliderTransform1.Shape == Box)
                {
                    if (colliderTransform2.Shape == Circle)
                    {
                        std::cout << "Collision type not defined" << std::endl;
                    }
                    else if (colliderTransform2.Shape == Box)
                    {
                        std::cout << "Collision type not defined" << std::endl;
                    }
                    else if (colliderTransform2.Shape == Convex)
                    {
                        std::cout << "Collision type not defined" << std::endl;
                    }
                }
                else if (colliderTransform1.Shape == Convex)
                {
                    if (colliderTransform2.Shape == Circle)
                    {
                        std::cout << "Collision type not defined" << std::endl;
                    }
                    else if (colliderTransform2.Shape == Box)
                    {
                        std::cout << "Collision type not defined" << std::endl;
                    }
                    else if (colliderTransform2.Shape == Convex)
                    {
                        std::cout << "Collision type not defined" << std::endl;
                    }
                }
            }
        }
    }

private:
    template <typename T>
    inline void swap(T& x, T& y)
    {
        T temp = x;
        x = y;
        y = temp;
    }
};