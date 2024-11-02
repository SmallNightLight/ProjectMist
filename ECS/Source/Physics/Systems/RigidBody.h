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
                    if (!colliderTransform1.IsDynamic)
                    {
                        colliderTransform2.MovePosition(resultInfo.Normal * resultInfo.Depth);
                    }
                    else if (!colliderTransform2.IsDynamic)
                    {
                        colliderTransform1.MovePosition(-resultInfo.Normal * resultInfo.Depth);
                    }
                    else
                    {
                        colliderTransform1.MovePosition(-resultInfo.Normal * resultInfo.Depth / 2);
                        colliderTransform2.MovePosition(resultInfo.Normal * resultInfo.Depth / 2);
                    }

                    ResolveCollision(entity1, entity2, colliderTransform1.IsDynamic, colliderTransform2.IsDynamic, resultInfo);
                }
            }
        }
    }

    void ApplyVelocity(Fixed16_16 deltaTime)
    {
        for (const Entity& entity : Entities)
        {
            ColliderTransform& colliderTransform = colliderTransformCollection->GetComponent(entity);

            if (colliderTransform.IsStatic) continue;

            RigidBodyData& rigidBodyData = rigidBodyDataCollection->GetComponent(entity);

            rigidBodyData.Velocity += rigidBodyData.Force * rigidBodyData.InverseMass * deltaTime;

            if (colliderTransform.IsDynamic)
            {
                rigidBodyData.Velocity += Gravity * deltaTime;
            }

            colliderTransform.MovePosition(rigidBodyData.Velocity * deltaTime);
            colliderTransform.Rotate(rigidBodyData.RotationalVelocity * deltaTime);

            rigidBodyData.Force = Vector2::Zero();
        }
    }

    void RotateAllEntities(Fixed16_16 delta) const
    {
        for (const Entity& entity : Entities)
        {
            RigidBodyData& rigidBodyData = rigidBodyDataCollection->GetComponent(entity);
            rigidBodyData.RotationalVelocity = Fixed16_16::pi() / 2;
        }
    }

    void WrapEntities(Camera camera)
    {
        for (const Entity& entity : Entities)
        {
            ColliderTransform& colliderTransform = colliderTransformCollection->GetComponent(entity);

            if (colliderTransform.Position.X < camera.Left) { colliderTransform.MovePosition(Vector2(camera.Width / camera.ZoomLevel, Fixed16_16(0))); }
            if (colliderTransform.Position.X > camera.Right) { colliderTransform.MovePosition(Vector2(-camera.Width / camera.ZoomLevel, Fixed16_16(0))); }
            if (colliderTransform.Position.Y < camera.Bottom) { colliderTransform.MovePosition(Vector2(Fixed16_16(0), camera.Height / camera.ZoomLevel)); }
            if (colliderTransform.Position.Y > camera.Top) { colliderTransform.MovePosition(Vector2(Fixed16_16(0), -camera.Height / camera.ZoomLevel)); }
        }
    }

private:
    bool DetectCollision(Entity entity1, Entity entity2, ColliderTransform colliderTransform1, ColliderTransform colliderTransform2, CollisionInfo& resultInfo)
    {
        //Skip if none of the objects are dynamic
        if (!colliderTransform1.IsDynamic && !colliderTransform2.IsDynamic)
        {
            return false;
        }

        //Check for different shape types and do the correct collision detection
        if (colliderTransform1.Shape == Circle)
        {
            if (colliderTransform2.Shape == Circle)
            {
                return collisionDetection.CircleCircleCollision(entity1, entity2, colliderTransform1, colliderTransform2, true, resultInfo);
            }
            if (colliderTransform2.Shape == Box)
            {
                return collisionDetection.CircleBoxCollisionDetection(entity1, entity2, colliderTransform1, colliderTransform2, false, resultInfo);
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
                return collisionDetection.CircleBoxCollisionDetection(entity2, entity1, colliderTransform2, colliderTransform1, true, resultInfo);
            }
            if (colliderTransform2.Shape == Box)
            {
                return collisionDetection.BoxBoxCollisionDetection(entity1, entity2, colliderTransform1, colliderTransform2, false, resultInfo);
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

    void ResolveCollision(Entity entity1, Entity entity2, bool isDynamic1, bool isDynamic2, const CollisionInfo& collisionInfo) //TODO: use owner and other in collisioninfo
    {
        RigidBodyData& rigidBodyData1= rigidBodyDataCollection->GetComponent(entity1);
        RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(entity2);

        Vector2 relativeVelocity = rigidBodyData2.Velocity - rigidBodyData1.Velocity;

        if (relativeVelocity.Dot(collisionInfo.Normal) > 0) return;

        Fixed16_16 inverseMass1 = isDynamic1 ? rigidBodyData1.InverseMass : Fixed16_16(0);
        Fixed16_16 inverseMass2 = isDynamic2 ? rigidBodyData2.InverseMass : Fixed16_16(0);

        Fixed16_16 restitution = min(rigidBodyData1.Restitution, rigidBodyData2.Restitution);
        Fixed16_16 j = (-(Fixed16_16(1) + restitution) * relativeVelocity.Dot(collisionInfo.Normal)) / (inverseMass1 + inverseMass2);
        Vector2 impulse = collisionInfo.Normal * j;

        rigidBodyData1.Velocity -= impulse * inverseMass1;
        rigidBodyData2.Velocity += impulse * inverseMass2;
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
};