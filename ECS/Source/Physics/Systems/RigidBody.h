#pragma once

class RigidBody : public System
{
public:
    explicit RigidBody(ECSWorld* world) : System(world), collisionDetection(World) //TODO: Static objects should not need to have a rigidBody
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
        std::vector<CollisionInfo> collisions;

        for (auto it1 = Entities.begin(); it1 != Entities.end(); ++it1)
        {
            const Entity& entity1 = *it1;
            ColliderTransform& colliderTransform1 = colliderTransformCollection->GetComponent(entity1);

            //Detect collisions
            for (auto it2 = std::next(it1); it2 != Entities.end(); ++it2)
            {
                const Entity& entity2 = *it2;
                ColliderTransform& colliderTransform2 = colliderTransformCollection->GetComponent(entity2);

                CollisionInfo resultInfo = CollisionInfo();

                if (collisionDetection.DetectCollisionAndCorrect(entity1, entity2, colliderTransform1, colliderTransform2, resultInfo))
                {
                    collisions.emplace_back(resultInfo);
                }
            }
        }

        for(CollisionInfo collision : collisions)
        {
            ResolveCollision(collision);
        }

        collisionsRE = std::vector(collisions);
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
    void ResolveCollision(const CollisionInfo& collisionInfo)
    {
        RigidBodyData& rigidBodyData1= rigidBodyDataCollection->GetComponent(collisionInfo.Entity1);
        RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(collisionInfo.Entity2);

        Vector2 relativeVelocity = rigidBodyData2.Velocity - rigidBodyData1.Velocity;

        if (relativeVelocity.Dot(collisionInfo.Normal) > 0) return;

        Fixed16_16 inverseMass1 = collisionInfo.IsDynamic1 ? rigidBodyData1.InverseMass : Fixed16_16(0);
        Fixed16_16 inverseMass2 = collisionInfo.IsDynamic2 ? rigidBodyData2.InverseMass : Fixed16_16(0);

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

public:
    std::vector<CollisionInfo> collisionsRE; //TODO: remove
};