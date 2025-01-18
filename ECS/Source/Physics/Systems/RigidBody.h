#pragma once

#include "../../ECS/ECS.h"
#include "../PhysicsSettings.h"

class RigidBody : public System
{
public:
    explicit RigidBody(Layer* world) : System(world), collisionDetection(layer) //TODO: Static objects should not need to have a rigidBody
    {
        colliderTransformCollection = layer->GetComponentCollection<ColliderTransform>();
        rigidBodyDataCollection = layer->GetComponentCollection<RigidBodyData>();
        circleColliderCollection = layer->GetComponentCollection<CircleCollider>();
        boxColliderCollection = layer->GetComponentCollection<BoxCollider>();
        polygonColliderCollection = world->GetComponentCollection<PolygonCollider>();
    }

    [[nodiscard]] Signature GetSignature() const override
    {
        Signature signature;
        signature.set(layer->GetComponentType<ColliderTransform>());
        signature.set(layer->GetComponentType<RigidBodyData>());
        return signature;
    }

    void DetectCollisions(uint32_t frame, uint32_t iteration, uint32_t id)
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

        if (LogCollisions && iteration == 0)
            CollisionInfo::LogCollisions(collisions, frame, id);

        for(CollisionInfo collision : collisions)
        {
            ResolveCollisionWithRotation(collision);
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
            colliderTransform.Rotate(rigidBodyData.AngularVelocity * deltaTime);

            rigidBodyData.Force = Vector2::Zero();
        }
    }

    void RotateAllEntities(Fixed16_16 delta) const
    {
        for (const Entity& entity : Entities)
        {
            RigidBodyData& rigidBodyData = rigidBodyDataCollection->GetComponent(entity);
            rigidBodyData.AngularVelocity = Fixed16_16::pi() / 2;
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
        Fixed16_16 velocityMagnitude = relativeVelocity.Dot(collisionInfo.Normal);

        if (velocityMagnitude > 0) return;

        Fixed16_16 inverseMass1 = collisionInfo.IsDynamic1 ? rigidBodyData1.InverseMass : Fixed16_16(0);
        Fixed16_16 inverseMass2 = collisionInfo.IsDynamic2 ? rigidBodyData2.InverseMass : Fixed16_16(0);

        Fixed16_16 restitution = min(rigidBodyData1.Restitution, rigidBodyData2.Restitution);
        Fixed16_16 j = -(Fixed16_16(1) + restitution) * velocityMagnitude / (inverseMass1 + inverseMass2);
        Vector2 impulse = collisionInfo.Normal * j;

        rigidBodyData1.Velocity -= impulse * inverseMass1;
        rigidBodyData2.Velocity += impulse * inverseMass2;
    }

    void ResolveCollisionWithRotation(const CollisionInfo& collisionInfo)
    {
        ColliderTransform colliderTransform1 = colliderTransformCollection->GetComponent(collisionInfo.Entity1);
        ColliderTransform colliderTransform2 = colliderTransformCollection->GetComponent(collisionInfo.Entity2);

        RigidBodyData& rigidBodyData1= rigidBodyDataCollection->GetComponent(collisionInfo.Entity1);
        RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(collisionInfo.Entity2);

        std::array<Vector2, 2> contacts {collisionInfo.Contact1, collisionInfo.Contact2};
        std::array<Vector2, 2> impulses { };
        std::array<Vector2, 2> r1 { };
        std::array<Vector2, 2> r2 { };
        std::array<Fixed16_16, 2> j { };

        Fixed16_16 restitution = min(rigidBodyData1.Restitution, rigidBodyData2.Restitution);

        Fixed16_16 inverseMass1 = collisionInfo.IsDynamic1 ? rigidBodyData1.InverseMass : Fixed16_16(0);
        Fixed16_16 inverseMass2 = collisionInfo.IsDynamic2 ? rigidBodyData2.InverseMass : Fixed16_16(0);

        Fixed16_16 inverseInertia1 = collisionInfo.IsDynamic1 ? rigidBodyData1.InverseInertia : Fixed16_16(0);
        Fixed16_16 inverseInertia2 = collisionInfo.IsDynamic2 ? rigidBodyData2.InverseInertia : Fixed16_16(0);

        Fixed16_16 staticFriction = (rigidBodyData1.StaticFriction + rigidBodyData2.StaticFriction) / 2;
        Fixed16_16 dynamicFriction = (rigidBodyData1.DynamicFriction + rigidBodyData2.DynamicFriction) / 2;

        for(int i = 0; i < collisionInfo.ContactCount; ++i)
        {
            r1[i] = contacts[i] - colliderTransform1.Position;
            r2[i] = contacts[i] - colliderTransform2.Position;

            Vector2 perpendicular1 = r1[i].Perpendicular();
            Vector2 perpendicular2 = r2[i].Perpendicular();

            Vector2 angularVelocity1 = perpendicular1 * rigidBodyData1.AngularVelocity;
            Vector2 angularVelocity2 = perpendicular2 * rigidBodyData2.AngularVelocity;

            Vector2 relativeVelocity = (rigidBodyData2.Velocity + angularVelocity2) - (rigidBodyData1.Velocity + angularVelocity1);

            Fixed16_16 velocityMagnitude = relativeVelocity.Dot(collisionInfo.Normal);

            if (velocityMagnitude > 0) continue;

            Fixed16_16 dot1 =  perpendicular1.Dot(collisionInfo.Normal);
            Fixed16_16 dot2 =  perpendicular2.Dot(collisionInfo.Normal);

            j[i] = -((Fixed16_16(1) + restitution) * velocityMagnitude / (inverseMass1 + inverseMass2 + dot1 * dot1 * inverseInertia1 + dot2 * dot2 * inverseInertia2)) / collisionInfo.ContactCount;
            impulses[i] = collisionInfo.Normal * j[i];
        }

        for(int i = 0; i < collisionInfo.ContactCount; ++i)
        {
            Vector2 impulse = impulses[i];

            rigidBodyData1.Velocity -= impulse * inverseMass1;
            rigidBodyData2.Velocity += impulse * inverseMass2;

            rigidBodyData1.AngularVelocity -= r1[i].Cross(impulse) * inverseInertia1;
            rigidBodyData2.AngularVelocity += r2[i].Cross(impulse) * inverseInertia2;
        }

        impulses = { };

        //Friction
        for(int i = 0; i < collisionInfo.ContactCount; ++i)
        {
            Vector2 perpendicular1 = r1[i].Perpendicular();
            Vector2 perpendicular2 = r2[i].Perpendicular();

            Vector2 angularVelocity1 = perpendicular1 * rigidBodyData1.AngularVelocity;
            Vector2 angularVelocity2 = perpendicular2 * rigidBodyData2.AngularVelocity;

            Vector2 relativeVelocity = (rigidBodyData2.Velocity + angularVelocity2) - (rigidBodyData1.Velocity + angularVelocity1);
            Vector2 tangent = relativeVelocity - collisionInfo.Normal * relativeVelocity.Dot(collisionInfo.Normal);

            if (Vector2::AlmostEqual(tangent, Vector2::Zero()))
            {
                continue;
            }

            tangent = tangent.Normalize();

            Fixed16_16 dot1 =  perpendicular1.Dot(tangent);
            Fixed16_16 dot2 =  perpendicular2.Dot(tangent);

            Fixed16_16 jT = -(relativeVelocity.Dot(tangent) / (inverseMass1 + inverseMass2 + dot1 * dot1 * inverseInertia1 + dot2 * dot2 * inverseInertia2)) / collisionInfo.ContactCount;

            Vector2 frictionImpulse;

            //Apply coulombs law
            if (fpm::abs(jT) <= j[i] * staticFriction)
            {
                frictionImpulse = tangent * jT;
            }
            else
            {
                frictionImpulse = tangent * (-j[i] * dynamicFriction);
            }

            impulses[i] = frictionImpulse;
        }

        for(int i = 0; i < collisionInfo.ContactCount; ++i)
        {
            Vector2 frictionImpulse = impulses[i];

            rigidBodyData1.Velocity -= frictionImpulse * inverseMass1;
            rigidBodyData2.Velocity += frictionImpulse * inverseMass2;

            rigidBodyData1.AngularVelocity -= r1[i].Cross(frictionImpulse) * inverseInertia1;
            rigidBodyData2.AngularVelocity += r2[i].Cross(frictionImpulse) * inverseInertia2;
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

    std::shared_ptr<ComponentCollection<ColliderTransform>> colliderTransformCollection;
    std::shared_ptr<ComponentCollection<RigidBodyData>> rigidBodyDataCollection;
    std::shared_ptr<ComponentCollection<CircleCollider>> circleColliderCollection;
    std::shared_ptr<ComponentCollection<BoxCollider>> boxColliderCollection;
    std::shared_ptr<ComponentCollection<PolygonCollider>> polygonColliderCollection;

public:
    std::vector<CollisionInfo> collisionsRE; //TODO: remove
};