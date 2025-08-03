#pragma once

#include "../../ECS/ECS.h"
#include "../PhysicsSettings.h"
#include "../Collision/CollisionCache.h"

#include <vector>
#include <tuple>

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

        collisionCache = nullptr;
    }

    void InitializeCache(CollisionCache* cache)
    {
        collisionCache = cache;
    }

    [[nodiscard]] Signature GetSignature() const override
    {
        Signature signature;
        signature.set(layer->GetComponentType<ColliderTransform>());
        signature.set(layer->GetComponentType<RigidBodyData>());
        return signature;
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

            //bool applyVelocity = rigidBodyData.Velocity.RawMagnitudeSquared() > VelocityEpsilon && rigidBodyData.Velocity.RawMagnitudeSquared() >= rigidBodyData.LastVelocity.RawMagnitudeSquared();
            //bool applyAngularVelocity = rigidBodyData.AngularVelocity > AngularVelocityEpsilon || rigidBodyData.AngularVelocity < -AngularVelocityEpsilon;

            rigidBodyData.LastPosition = colliderTransform.Position;
            rigidBodyData.LastVelocity = rigidBodyData.Velocity;
            rigidBodyData.LastAngularVelocity = rigidBodyData.AngularVelocity;

            colliderTransform.MovePosition(rigidBodyData.Velocity * deltaTime);
            rigidBodyData.Force = Vector2::Zero();

            colliderTransform.Rotate(rigidBodyData.AngularVelocity * deltaTime);
        }
    }

    void HandleCollisions(FrameNumber frame, uint8_t iteration, uint32_t id)
    {
        assert(collisionCache && "CollisionCache is null");

        currentFrameNumber = PhysicsIterations * frame + iteration;
        std::vector<CollisionInfo> collisions;
        std::vector<CollisionCheckInfo> collisionChecks;

        for (auto it1 = Entities.begin(); it1 != Entities.end(); ++it1)
        {
            const Entity& entity1 = *it1;
            ColliderTransform& colliderTransform1 = colliderTransformCollection->GetComponent(entity1);
            RigidBodyData& rigidBodyData1= rigidBodyDataCollection->GetComponent(entity1);

            //Detect collisions
            for (auto it2 = std::next(it1); it2 != Entities.end(); ++it2)
            {
                const Entity& entity2 = *it2;
                ColliderTransform& colliderTransform2 = colliderTransformCollection->GetComponent(entity2);
                RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(entity2);

                CollisionCheckInfo check = CollisionCheckInfo();

                check.Entity1 = entity1;
                check.Entity2 = entity2;
                check.Rotation1 = colliderTransform1.Rotation;
                check.Rotation2 = colliderTransform2.Rotation;
                check.GravityScale1 = rigidBodyData1.GravityScale;
                check.GravityScale2 = rigidBodyData2.GravityScale;
                check.MassScale1 = rigidBodyData1.GravityScale;
                check.MassScale2 = rigidBodyData2.GravityScale;

                if (colliderTransform1.IsStatic)
                {
                    check.Position1 = colliderTransform1.Position;
                    check.Velocity1 = Vector2(0, 0);
                    check.AngularVelocity1 = Fixed16_16(0);
                }
                else
                {
                    check.Position1 = rigidBodyData1.LastPosition;
                    check.Velocity1 = rigidBodyData1.LastVelocity;
                    check.AngularVelocity1 = rigidBodyData1.LastAngularVelocity;
                }

                if (colliderTransform2.IsStatic)
                {
                    check.Position2 = colliderTransform2.Position;
                    check.Velocity2 = Vector2(0, 0);
                    check.AngularVelocity2 = Fixed16_16(0);
                }
                else
                {
                    check.Position2 = rigidBodyData2.LastPosition;
                    check.Velocity2 = rigidBodyData2.LastVelocity;
                    check.AngularVelocity2 = rigidBodyData2.LastAngularVelocity;
                }

                //TODO: Try to cache some things to avoid rigid body access

                //Check if collision already occurred in the past
                /**/

                CollisionResponseInfo responseInfo;
                if (collisionCache->TryGetCollisionData(currentFrameNumber, check, responseInfo))
                {
                    if (responseInfo.Collision)
                    {
                        colliderTransform1.Position = responseInfo.Position1;
                        colliderTransform2.Position = responseInfo.Position2;
                        rigidBodyData1.Velocity = responseInfo.Velocity1;
                        rigidBodyData2.Velocity = responseInfo.Velocity2;
                        rigidBodyData1.AngularVelocity = responseInfo.AngularVelocity1;
                        rigidBodyData2.AngularVelocity = responseInfo.AngularVelocity2;
                    }

                    continue;
                }
                /**/

                CollisionInfo resultInfo = CollisionInfo();
                if (collisionDetection.DetectCollisionAndCorrect(entity1, entity2, colliderTransform1, colliderTransform2, resultInfo))
                {
                    collisions.emplace_back(resultInfo);
                    collisionChecks.emplace_back(check);
                }
                else
                {
                    //Cache result
                    collisionCache->Cache(currentFrameNumber, check, NonColliding);
                }
            }
        }
        //TODO: Does not cache if one object is static. Cache objects that dont collide in different structure?
        //TODO: AND ONLY try to get the value if in a rollback and we know there is data
        //TODO: Only first frame first iteration works
        //solution: we dont move the object from the cache like before in the collision detection moving

        if (LogCollisions && iteration == 0)
        {
            CollisionInfo::LogCollisions(collisions, frame, id);
        }

        for (size_t i = 0; i < collisions.size(); ++i)
        {
            ResolveCollision(collisions[i], collisionChecks[i]);
        }

        collisionsRE = std::vector(collisions);
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
    //Resolves the collision without rotation
    void ResolveCollisionBasic(const CollisionInfo& collisionInfo)
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

    void ResolveCollision(const CollisionInfo& collisionInfo, const CollisionCheckInfo& check)
    {
        assert(collisionCache && "CollisionCache is null");

        ColliderTransform colliderTransform1 = colliderTransformCollection->GetComponent(collisionInfo.Entity1);
        ColliderTransform colliderTransform2 = colliderTransformCollection->GetComponent(collisionInfo.Entity2);

        RigidBodyData& rigidBodyData1 = rigidBodyDataCollection->GetComponent(collisionInfo.Entity1);
        RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(collisionInfo.Entity2);

        Vector2 newVelocity1 = rigidBodyData1.Velocity;
        Vector2 newVelocity2 = rigidBodyData2.Velocity;
        Fixed16_16 newAngularVelocity1 = rigidBodyData1.AngularVelocity;
        Fixed16_16 newAngularVelocity2 = rigidBodyData2.AngularVelocity;

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

            Vector2 relativeVelocity = (newVelocity2 + perpendicular2 * newAngularVelocity2) - (newVelocity1 + perpendicular1 * newAngularVelocity1);
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

            newVelocity1 -= impulse * inverseMass1;
            newVelocity2 += impulse * inverseMass2;

            newAngularVelocity1 -= r1[i].Cross(impulse) * inverseInertia1;
            newAngularVelocity2 += r2[i].Cross(impulse) * inverseInertia2;
        }

        impulses = { };

        //Friction
        for(int i = 0; i < collisionInfo.ContactCount; ++i)
        {
            Vector2 perpendicular1 = r1[i].Perpendicular();
            Vector2 perpendicular2 = r2[i].Perpendicular();

            Vector2 relativeVelocity = (newVelocity2 + perpendicular2 * newAngularVelocity2) - (newVelocity1 + perpendicular1 * newAngularVelocity1);
            Vector2 tangent = relativeVelocity - collisionInfo.Normal * relativeVelocity.Dot(collisionInfo.Normal);

            if (Vector2::AlmostEqual(tangent, Vector2::Zero())) continue;

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

            newVelocity1 -= frictionImpulse * inverseMass1;
            newVelocity2 += frictionImpulse * inverseMass2;

            newAngularVelocity1 -= r1[i].Cross(frictionImpulse) * inverseInertia1;
            newAngularVelocity2 += r2[i].Cross(frictionImpulse) * inverseInertia2;
        }

        //Cache result
        /**/
        CollisionResponseInfo responseInfo = CollisionResponseInfo(true, colliderTransform1.Position, colliderTransform2.Position, newVelocity1, newVelocity2, newAngularVelocity1, newAngularVelocity2);
        collisionCache->Cache(currentFrameNumber, check, responseInfo);
        /**/

        //Apply velocities
        rigidBodyData1.Velocity = newVelocity1;
        rigidBodyData2.Velocity = newVelocity2;
        rigidBodyData1.AngularVelocity = newAngularVelocity1;
        rigidBodyData2.AngularVelocity = newAngularVelocity2;
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
    FrameNumber currentFrameNumber;
    CollisionDetection collisionDetection;

    std::shared_ptr<ComponentCollection<ColliderTransform>> colliderTransformCollection;
    std::shared_ptr<ComponentCollection<RigidBodyData>> rigidBodyDataCollection;
    std::shared_ptr<ComponentCollection<CircleCollider>> circleColliderCollection;
    std::shared_ptr<ComponentCollection<BoxCollider>> boxColliderCollection;
    std::shared_ptr<ComponentCollection<PolygonCollider>> polygonColliderCollection;

    //Caching
    CollisionCache* collisionCache;

public:
    std::vector<CollisionInfo> collisionsRE; //TODO: remove
};