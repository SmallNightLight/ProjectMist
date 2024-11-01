#pragma once

class RigidBody : public System
{
public:
    RigidBody()
    {
        colliderTransformCollection = EcsManager.GetComponentCollection<ColliderTransform>();
        rigidBodyDataCollection = EcsManager.GetComponentCollection<RigidBodyData>();
        circleColliderCollection = EcsManager.GetComponentCollection<CircleCollider>();
        boxColliderCollection = EcsManager.GetComponentCollection<BoxCollider>();
    }

    static Signature GetSignature()
    {
        Signature signature;
        signature.set(EcsManager.GetComponentType<ColliderTransform>());
        signature.set(EcsManager.GetComponentType<RigidBodyData>());
        return signature;
    }

    void Update()
    {
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
                            MovePosition(entity1, collisionInfo.Normal * collisionInfo.Depth / 2, colliderTransform1);
                            MovePosition(entity2, -collisionInfo.Normal * collisionInfo.Depth / 2, colliderTransform2);
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
    static std::array<Vector2, 4> GetTransformedVertices(ColliderTransform& colliderTransform, BoxCollider& boxCollider)
    {
        if (boxCollider.TransformUpdateRequired)
        {
            for (int i = 0; i < 4; ++i)
            {
                boxCollider.TransformedVertices[i] = colliderTransform.Transform(boxCollider.Vertices[i]);
            }

            boxCollider.TransformUpdateRequired = false;
        }

        return boxCollider.TransformedVertices;
    }

    inline void MovePosition(Entity entity, const Vector2& direction, ColliderTransform& colliderTransform) const
    {
        colliderTransform.MovePosition(direction);

        if (colliderTransform.Shape == Box)
        {
            boxColliderCollection->GetComponent(entity).TransformUpdateRequired = true;
        }
    }

    inline void SetPosition(Entity entity, const Vector2& newPosition, ColliderTransform& colliderTransform) const
    {
        colliderTransform.SetPosition(newPosition);

        if (colliderTransform.Shape == Box)
        {
            boxColliderCollection->GetComponent(entity).TransformUpdateRequired = true;
        }
    }

    inline void Rotate(Entity entity, const Fixed16_16& amount, ColliderTransform& colliderTransform) const
    {
        colliderTransform.Rotate(amount);

        if (colliderTransform.Shape == Box)
        {
            boxColliderCollection->GetComponent(entity).TransformUpdateRequired = true;
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

private:
    ComponentCollection<ColliderTransform>* colliderTransformCollection;
    ComponentCollection<RigidBodyData>* rigidBodyDataCollection;
    ComponentCollection<CircleCollider>* circleColliderCollection;
    ComponentCollection<BoxCollider>* boxColliderCollection;
};