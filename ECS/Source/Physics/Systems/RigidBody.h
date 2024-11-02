#pragma once

class RigidBody : public System
{
public:
    explicit RigidBody(ECSWorld* world) : System(world)
    {
        colliderTransformCollection = World->GetComponentCollection<ColliderTransform>();
        rigidBodyDataCollection = World->GetComponentCollection<RigidBodyData>();
        circleColliderCollection = World->GetComponentCollection<CircleCollider>();
        boxColliderCollection = World->GetComponentCollection<BoxCollider>();

        renderDataCollection = World->GetComponentCollection<ColliderRenderData>();
    }

    [[nodiscard]] Signature GetSignature() const
    {
        Signature signature;
        signature.set(World->GetComponentType<ColliderTransform>());
        signature.set(World->GetComponentType<RigidBodyData>());
        return signature;
    }

    void Update()
    {
        for (const Entity& entity : Entities)
        {
            renderDataCollection->GetComponent(entity).Outline = false;
        }

        auto collisionDetection = CollisionDetection(World);
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
                        if (collisionDetection.CircleBoxCollisionDetection(entity1, entity2, colliderTransform1, colliderTransform2, collisionInfo))
                        {
                            MovePosition(entity1, -collisionInfo.Normal * collisionInfo.Depth / 2, colliderTransform1);
                            MovePosition(entity2, collisionInfo.Normal * collisionInfo.Depth / 2, colliderTransform2);
                            renderDataCollection->GetComponent(entity1).Outline = true;
                        }
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
                        if (collisionDetection.CircleBoxCollisionDetection(entity2, entity1, colliderTransform2, colliderTransform1, collisionInfo))
                        {
                            MovePosition(entity1, collisionInfo.Normal * collisionInfo.Depth / 2, colliderTransform1);
                            MovePosition(entity2, -collisionInfo.Normal * collisionInfo.Depth / 2, colliderTransform2);
                            renderDataCollection->GetComponent(entity1).Outline = true;
                        }
                    }
                    else if (colliderTransform2.Shape == Box)
                    {
                        if (collisionDetection.BoxBoxCollisionDetection(entity1, entity2, colliderTransform1, colliderTransform2, collisionInfo))
                        {
                            MovePosition(entity1, -collisionInfo.Normal * collisionInfo.Depth / 2, colliderTransform1);
                            MovePosition(entity2, collisionInfo.Normal * collisionInfo.Depth / 2, colliderTransform2);
                        }
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

    void RotateAllEntities(Fixed16_16 delta) const
    {
        for (const Entity& entity : Entities)
        {
            Rotate(entity, Fixed16_16::pi() / 2 * delta /100, colliderTransformCollection->GetComponent(entity));
        }
    }

private:
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

    ComponentCollection<ColliderRenderData>* renderDataCollection;
};