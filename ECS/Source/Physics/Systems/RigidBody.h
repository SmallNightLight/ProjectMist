#pragma once

class RigidBody : public System
{
public:
    explicit RigidBody(ECSWorld* world) : System(world), collisionDetection(World)
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

    void DetectCollisions()
    {
        for (const Entity& entity : Entities)
        {
            renderDataCollection->GetComponent(entity).Outline = false;
        }

        CollisionInfo resultInfo(Vector2::Zero(), 0, 0, Fixed16_16(0));

        for (auto it1 = Entities.begin(); it1 != Entities.end(); ++it1)
        {
            const Entity& entity1 = *it1;
            ColliderTransform& colliderTransform1 = colliderTransformCollection->GetComponent(entity1);

            //Detect collisions
            for (auto it2 = std::next(it1); it2 != Entities.end(); ++it2)
            {
                const Entity& entity2 = *it2;
                ColliderTransform& colliderTransform2 = colliderTransformCollection->GetComponent(entity2);

                if (DetectCollision(entity1, entity2, colliderTransform1, colliderTransform2, resultInfo)) //SWAPPING
                {
                    MovePosition(entity1, resultInfo.Normal * resultInfo.Depth / 2, colliderTransform1);
                    MovePosition(entity2, -resultInfo.Normal * resultInfo.Depth / 2, colliderTransform2);
                    renderDataCollection->GetComponent(entity1).Outline = true;
                }
            }
        }
    }

    void ApplyVelocity(Fixed16_16 deltaTime)
    {
        for (const Entity& entity : Entities)
        {
            ColliderTransform& colliderTransform = colliderTransformCollection->GetComponent(entity);
            RigidBodyData& rigidBodyData = rigidBodyDataCollection->GetComponent(entity);

            colliderTransform.MovePosition(rigidBodyData.Velocity * deltaTime);
            colliderTransform.Rotate(rigidBodyData.RotationalVelocity * deltaTime);
        }
    }

    void RotateAllEntities(Fixed16_16 delta) const
    {
        for (const Entity& entity : Entities)
        {
            Rotate(entity, Fixed16_16::pi() / 2 * delta, colliderTransformCollection->GetComponent(entity));
        }
    }

private:
    bool DetectCollision(Entity entity1, Entity entity2, ColliderTransform colliderTransform1, ColliderTransform colliderTransform2, CollisionInfo& resultInfo)
    {
        //Check for different shape types and do the correct collision detection
        if (colliderTransform1.Shape == Circle)
        {
            if (colliderTransform2.Shape == Circle)
            {
                return collisionDetection.CircleCircleCollision(entity1, entity2, colliderTransform1, colliderTransform2, false, resultInfo);
            }
            if (colliderTransform2.Shape == Box)
            {
                return collisionDetection.CircleBoxCollisionDetection(entity1, entity2, colliderTransform1, colliderTransform2, true, resultInfo);
            }
            if (colliderTransform2.Shape == Convex)
            {
                std::cout << "Collision type not defined" << std::endl;
            }
        }
        else if (colliderTransform1.Shape == Box)
        {
            if (colliderTransform2.Shape == Circle)
            {
                return collisionDetection.CircleBoxCollisionDetection(entity2, entity1, colliderTransform2, colliderTransform1, false, resultInfo);
            }
            if (colliderTransform2.Shape == Box)
            {
                return collisionDetection.BoxBoxCollisionDetection(entity1, entity2, colliderTransform1, colliderTransform2, true, resultInfo);
            }
            if (colliderTransform2.Shape == Convex)
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

        return false;
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
    CollisionDetection collisionDetection;

    ComponentCollection<ColliderTransform>* colliderTransformCollection;
    ComponentCollection<RigidBodyData>* rigidBodyDataCollection;
    ComponentCollection<CircleCollider>* circleColliderCollection;
    ComponentCollection<BoxCollider>* boxColliderCollection;

    ComponentCollection<ColliderRenderData>* renderDataCollection;
};