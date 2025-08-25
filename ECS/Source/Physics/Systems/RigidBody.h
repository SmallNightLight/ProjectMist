#pragma once

#include "../../ECS/ECS.h"
#include "../PhysicsSettings.h"
#include "../Collision/CollisionDetection.h"
#include "../Collision/CollisionCache.h"
#include "../Collision/PhysicsCache.h"

#include <vector>

class RigidBody
{
public:
    using RequiredComponents = ComponentList<ColliderTransform, RigidBodyData>;

    explicit RigidBody(PhysicsComponentManager& componentManager) : collisionDetection(componentManager) //TODO: Static objects should not need to have a rigidBody
    {
        currentFrameNumber = 0;

        colliderTransformCollection = componentManager.GetComponentCollection<ColliderTransform>();
        rigidBodyDataCollection = componentManager.GetComponentCollection<RigidBodyData>();
        circleColliderCollection = componentManager.GetComponentCollection<CircleCollider>();
        boxColliderCollection = componentManager.GetComponentCollection<BoxCollider>();
        polygonColliderCollection = componentManager.GetComponentCollection<PolygonCollider>();

        collisionCache = nullptr;
        physicsCache = nullptr;

        Entities.Initialize();
    }

    void InitializeCache(CollisionCache* pCollisionCache, PhysicsCache* pPhysicsCache)
    {
        collisionCache = pCollisionCache;
        physicsCache = pPhysicsCache;
    }

    void HandleCollisions(FrameNumber frame, uint32_t id)
    {
        assert(collisionCache && "CollisionCache is null");

        //Update & Validate collision cache
        currentFrameNumber = frame;
        bool useCache = collisionCache->UpdateFrame(currentFrameNumber);

        ContactPairs.clear();
        CollisionChecks.clear();

        for (Entity* it1 = Entities.begin(); it1 != Entities.end(); ++it1)
        {
            const Entity& entity1 = *it1;
            ColliderTransform& colliderTransform1 = colliderTransformCollection->GetComponent(entity1);
            uint32_t hash1 = colliderTransform1.GetHash(entity1);

            //Detect collisions
            for (Entity* it2 = std::next(it1); it2 != Entities.end(); ++it2)
            {
                const Entity& entity2 = *it2;

                //Entities should be ordered

                ColliderTransform& colliderTransform2 = colliderTransformCollection->GetComponent(entity2);
                uint32_t hash2 = colliderTransform2.GetHash(entity2);

                CollisionPairData pairData = CollisionPairData(entity1, entity2, colliderTransform1.Position, colliderTransform2.Position, colliderTransform1.Rotation, colliderTransform2.Rotation, hash1, hash2);

                //Check if collision already occurred in the past
                bool foundCollision = false;

                if (useCache)
                {
                    bool collision;
                    foundCollision = collisionCache->TryGetPairData(currentFrameNumber, pairData, collision);

                    if (foundCollision && !collision)
                        continue;
                }

                CollisionCheckInfo check;

                RigidBodyData& rigidBodyData1 = rigidBodyDataCollection->GetComponent(entity1);
                RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(entity2);

                if (foundCollision) //todo add ordering to cache
                {
                    //Get collision response data
                    check = GetCollisionCheckInfo(entity1, entity2, rigidBodyData1, rigidBodyData2);

                    ContactPair cP;
                    if (collisionCache->TryGetCollisionData(currentFrameNumber, check, cP))
                    {
                        ContactPairs.emplace_back(cP);
                        continue;
                    }
                }

                ContactPair contactPair = ContactPair();    //Value initialization to give the impulses zero values
                if (collisionDetection.DetectCollision(entity1, entity2, colliderTransform1, colliderTransform2, contactPair))
                {
                    SetImpulses(contactPair);
                    ContactPairs.emplace_back(contactPair);
                    collisionCache->CacheCollisionPair(currentFrameNumber, pairData);

                    if (!foundCollision)
                    {
                        check = GetCollisionCheckInfo(entity1, entity2, rigidBodyData1, rigidBodyData2);
                    }

                    CollisionChecks.emplace_back(check);
                    collisionCache->CacheCollision(currentFrameNumber, check, contactPair);
                }
                else
                {
                    //Cache result
                    collisionCache->CacheNonCollision(currentFrameNumber, pairData);
                }
            }
        }

        if (LogCollisions)
        {
            //CollisionInfo::LogCollisions(ContactPair, frame, id); todo
        }
    }

    static inline CollisionCheckInfo GetCollisionCheckInfo(Entity entity1, Entity entity2, RigidBodyData& rigidBodyData1, RigidBodyData& rigidBodyData2)
    {
        return CollisionCheckInfo(entity1, entity2, rigidBodyData1, rigidBodyData2);

        //Version 2 (currently slower) - no caching
        // return CollisionCheckInfo(
        //     entity1, entity2,
        //     rigidBodyData1.Velocity, rigidBodyData2.Velocity,
        //     rigidBodyData1.AngularVelocity, rigidBodyData2.AngularVelocity,
        //     rigidBodyData1.GravityScale, rigidBodyData2.GravityScale,
        //     rigidBodyData1.MassScale, rigidBodyData2.MassScale);
    }

    inline void SetImpulses(ContactPair& contactPair) const
    {
        if (contactPair.ContactCount == 0) return;

        EntityPair entityPair = EntityPair::Make(contactPair.Entity1, contactPair.Entity2);
        ImpulseData lastImpulseData;

        ImpulseData newImpulseData;
        newImpulseData.EntityKey = entityPair;
        newImpulseData.ContactCount = contactPair.ContactCount;

        if (physicsCache->TryGetImpulses(entityPair, lastImpulseData))
        {
            for (uint8_t i = 0; i < contactPair.ContactCount; ++i)
            {
                Contact& newContact = contactPair.Contacts[i];
                for (uint8_t j = 0; j < lastImpulseData.ContactCount; ++j)
                {
                    if (newContact.LastImpulse.Feature.Value == lastImpulseData.LastImpulses[j].Feature.Value)
                    {
                        //Warm starting
                        newContact.LastImpulse = lastImpulseData.LastImpulses[j];
                        break;
                    }
                }
            }
        }
    }

    void IntegrateForces(Fixed16_16 deltaTime)
    {
        for (const Entity& entity : Entities)
        {
            ColliderTransform& colliderTransform = colliderTransformCollection->GetComponent(entity);

            if (colliderTransform.IsStatic) continue;

            RigidBodyData& rigidBodyData = rigidBodyDataCollection->GetComponent(entity);

            rigidBodyData.Velocity += (Gravity + rigidBodyData.Force * rigidBodyData.InverseMass) * deltaTime;
            //rigidBodyData.AngularVelocity += deltaTime * rigidBodyData.InverseInertia * rigidBodyData.Torque; //todo
        }
    }

    static constexpr bool AccumulateImpulses = true;

    void SetupContacts(Fixed16_16 inverseDelta)
    {
        constexpr Fixed16_16 k_allowedPenetration = Fixed16_16(1) / Fixed16_16(100);
        constexpr Fixed16_16 k_biasFactor = Fixed16_16(0, 0);

        for (ContactPair& contactPair : ContactPairs)
        {
            RigidBodyData& rigidBodyData1 = rigidBodyDataCollection->GetComponent(contactPair.Entity1);
            RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(contactPair.Entity2);

            for (int i = 0; i < contactPair.ContactCount; ++i)
            {
                Contact& contact = contactPair.Contacts[i];

                //Precompute normal mass, tangent mass, and bias.
                Fixed16_16 rn1 = contact.R1.Dot(contactPair.Normal);
                Fixed16_16 rn2 = contact.R2.Dot(contactPair.Normal);
                Fixed16_16 kNormal = rigidBodyData1.InverseMass + rigidBodyData2.InverseMass;
                kNormal += rigidBodyData1.InverseInertia * (contact.R1.Dot(contact.R1) - rn1 * rn1) + rigidBodyData2.InverseInertia * (contact.R2.Dot(contact.R2) - rn2 * rn2);
                contact.MassNormal = Fixed16_16(1) / kNormal;

                Vector2 tangent = contactPair.Normal.Perpendicular();
                Fixed16_16 rt1 = contact.R1.Dot(tangent);
                Fixed16_16 rt2 = contact.R2.Dot(tangent);
                Fixed16_16 kTangent = rigidBodyData1.InverseMass + rigidBodyData2.InverseMass;
                kTangent += rigidBodyData1.InverseInertia * (contact.R1.Dot(contact.R1) - rt1 * rt1) + rigidBodyData2.InverseInertia * (contact.R2.Dot(contact.R2) - rt2 * rt2);
                contact.MassTangent = Fixed16_16(1) / kTangent;

                //contact.Bias = -k_biasFactor * inverseDelta * fpm::min(Fixed16_16(0), contact.Separation + k_allowedPenetration);

                if (AccumulateImpulses)
                {
                    //Apply normal + friction impulse
                    Vector2 P = contactPair.Normal * contact.LastImpulse.Pn + tangent * contact.LastImpulse.Pt;

                    rigidBodyData1.Velocity -= P * rigidBodyData1.InverseMass;
                    rigidBodyData1.AngularVelocity -= rigidBodyData1.InverseInertia * contact.R1.Cross(P);

                    rigidBodyData2.Velocity += P * rigidBodyData2.InverseMass;
                    rigidBodyData2.AngularVelocity += rigidBodyData2.InverseInertia * contact.R2.Cross(P);
                }
            }
        }
    }

    void SolveContacts()
    {
        for (ContactPair& contactPair : ContactPairs)
        {
            RigidBodyData& rigidBodyData1 = rigidBodyDataCollection->GetComponent(contactPair.Entity1);
            RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(contactPair.Entity2);

            for (int i = 0; i < contactPair.ContactCount; ++i)
            {
                Contact& contact = contactPair.Contacts[i];

                //Relative velocity at contact
                Vector2 dv = rigidBodyData2.Velocity + contact.R2.CrossI(rigidBodyData2.AngularVelocity) - rigidBodyData1.Velocity - contact.R1.CrossI(rigidBodyData1.AngularVelocity);

                //Compute normal impulse
                Fixed16_16 vn = dv.Dot(contactPair.Normal);

                Fixed16_16 dPn = contact.MassNormal * (-vn); //todo check if bias ofr position correction (-vn + contact.Bias);
                if (AccumulateImpulses)
                {
                    //Clamp the accumulated impulse
                    Fixed16_16 Pn0 = contact.LastImpulse.Pn;
                    contact.LastImpulse.Pn = fpm::max(Pn0 + dPn, Fixed16_16(0));
                    dPn = contact.LastImpulse.Pn - Pn0;
                }
                else
                {
                    dPn = fpm::max(dPn, Fixed16_16(0));
                }

                //Apply contact impulse
                Vector2 Pn = contactPair.Normal * dPn;

                rigidBodyData1.Velocity -= Pn * rigidBodyData1.InverseMass;
                rigidBodyData1.AngularVelocity -= rigidBodyData1.InverseInertia * contact.R1.Cross(Pn);

                rigidBodyData2.Velocity += Pn * rigidBodyData2.InverseMass;
                rigidBodyData2.AngularVelocity += rigidBodyData2.InverseInertia * contact.R2.Cross(Pn);

                //Relative velocity at contact
                dv = rigidBodyData2.Velocity + contact.R2.CrossI(rigidBodyData2.AngularVelocity) - rigidBodyData1.Velocity - contact.R1.CrossI(rigidBodyData1.AngularVelocity);

                Vector2 tangent = contactPair.Normal.Perpendicular();
                Fixed16_16 vt = dv.Dot(tangent);
                Fixed16_16 dPt = contact.MassTangent * -vt;

                if (AccumulateImpulses)
                {
                    //Compute friction impulse
                    Fixed16_16 maxPt = contactPair.Friction *  contact.LastImpulse.Pn;

                    //Clamp friction
                    Fixed16_16 oldTangentImpulse =  contact.LastImpulse.Pt;
                    contact.LastImpulse.Pt = clamp(oldTangentImpulse + dPt, -maxPt, maxPt);
                    dPt =  contact.LastImpulse.Pt - oldTangentImpulse;
                }
                else
                {
                    Fixed16_16 maxPt = contactPair.Friction * dPn;
                    dPt = clamp(dPt, -maxPt, maxPt);
                }

                //Apply contact impulse
                Vector2 Pt = tangent * dPt;

                rigidBodyData1.Velocity -= Pt * rigidBodyData1.InverseMass;
                rigidBodyData1.AngularVelocity -= rigidBodyData1.InverseInertia * contact.R1.Cross(Pt);

                rigidBodyData2.Velocity += Pt * rigidBodyData2.InverseMass;
                rigidBodyData2.AngularVelocity += rigidBodyData2.InverseInertia * contact.R2.Cross(Pt);
            }
        }
    }

    void IntegrateVelocities(Fixed16_16 deltaTime)
    {
        for (const Entity& entity : Entities)
        {
            ColliderTransform& colliderTransform = colliderTransformCollection->GetComponent(entity);

            if (colliderTransform.IsStatic) continue;

            RigidBodyData& rigidBodyData = rigidBodyDataCollection->GetComponent(entity);

            colliderTransform.MovePosition(rigidBodyData.Velocity * deltaTime);
            colliderTransform.Rotate(rigidBodyData.AngularVelocity * deltaTime);

            rigidBodyData.Force = Vector2(0, 0);
            //rigidBodyData.Torque = Fixed16_16(0); //todo
        }
    }

    void IntegratePositions()
    {
        physicsCache->Flip();

        for (ContactPair& contactPair : ContactPairs)
        {
            ColliderTransform& colliderTransform1 = colliderTransformCollection->GetComponent(contactPair.Entity1);
            ColliderTransform& colliderTransform2 = colliderTransformCollection->GetComponent(contactPair.Entity2);
            RigidBodyData& rigidBodyData1 = rigidBodyDataCollection->GetComponent(contactPair.Entity1);
            RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(contactPair.Entity2);

            for (Contact& contact : contactPair.Contacts)
            {
                //Position correction
                constexpr Fixed16_16 steeringConstant = Fixed16_16(0, 5);
                constexpr Fixed16_16 maxCorrection = -Fixed16_16(5);
                constexpr Fixed16_16 slop = Fixed16_16(1) / Fixed16_16(100);

                Fixed16_16 steeringForce = clamp(steeringConstant * (contact.Separation + slop), maxCorrection, Fixed16_16(0));
                Vector2 impulse = contactPair.Normal * (-steeringForce * contact.MassNormal);

                if (!colliderTransform1.IsStatic)
                {
                    colliderTransform1.MovePosition(-impulse * rigidBodyData1.InverseMass);
                    Vector2 r1 = contact.Position - colliderTransform1.Position;
                    colliderTransform1.Rotate(-r1.Cross(impulse) * rigidBodyData1.InverseInertia);
                }

                if (!colliderTransform2.IsStatic)
                {
                    colliderTransform2.MovePosition(impulse * rigidBodyData2.InverseMass);
                    Vector2 r2 = contact.Position - colliderTransform2.Position;
                    colliderTransform2.Rotate(r2.Cross(impulse) * rigidBodyData2.InverseInertia);
                }
            }

            //Cache impulses
            ImpulseData newImpulses;
            newImpulses.EntityKey = EntityPair::Make(contactPair.Entity1, contactPair.Entity2);
            newImpulses.ContactCount = contactPair.ContactCount;

            for (uint8_t i = 0; i < contactPair.ContactCount; ++i)
            {
                newImpulses.LastImpulses[i] = contactPair.Contacts[i].LastImpulse;
            }

            physicsCache->Cache(newImpulses);
        }
    }

    // void ResolveCollision(const ContactPair& contactPair, const CollisionCheckInfo& check, bool cacheResults)
    // {
    //     assert(collisionCache && "CollisionCache is null");
    //
    //     ColliderTransform colliderTransform1 = colliderTransformCollection->GetComponent(contactPair.Entity1);
    //     ColliderTransform colliderTransform2 = colliderTransformCollection->GetComponent(contactPair.Entity2);
    //
    //     RigidBodyData& rigidBodyData1 = rigidBodyDataCollection->GetComponent(contactPair.Entity1);
    //     RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(contactPair.Entity2);
    //
    //     Vector2 newVelocity1 = rigidBodyData1.Velocity;
    //     Vector2 newVelocity2 = rigidBodyData2.Velocity;
    //     Fixed16_16 newAngularVelocity1 = rigidBodyData1.AngularVelocity;
    //     Fixed16_16 newAngularVelocity2 = rigidBodyData2.AngularVelocity;
    //
    //     std::array<Vector2, 2> contacts {collisionInfo.Contact1, collisionInfo.Contact2};
    //     std::array<Vector2, 2> impulses { };
    //     std::array<Vector2, 2> r1 { };
    //     std::array<Vector2, 2> r2 { };
    //     std::array<Fixed16_16, 2> j { };
    //
    //     Fixed16_16 restitution = min(rigidBodyData1.Restitution, rigidBodyData2.Restitution);
    //
    //     Fixed16_16 inverseMass1 = collisionInfo.IsDynamic1 ? rigidBodyData1.InverseMass : Fixed16_16(0);
    //     Fixed16_16 inverseMass2 = collisionInfo.IsDynamic2 ? rigidBodyData2.InverseMass : Fixed16_16(0);
    //
    //     Fixed16_16 inverseInertia1 = collisionInfo.IsDynamic1 ? rigidBodyData1.InverseInertia : Fixed16_16(0);
    //     Fixed16_16 inverseInertia2 = collisionInfo.IsDynamic2 ? rigidBodyData2.InverseInertia : Fixed16_16(0);
    //
    //     Fixed16_16 staticFriction = (rigidBodyData1.StaticFriction + rigidBodyData2.StaticFriction) / 2;
    //     Fixed16_16 dynamicFriction = (rigidBodyData1.DynamicFriction + rigidBodyData2.DynamicFriction) / 2;
    //
    //     for(int i = 0; i < collisionInfo.ContactCount; ++i)
    //     {
    //         r1[i] = contacts[i] - colliderTransform1.Position;
    //         r2[i] = contacts[i] - colliderTransform2.Position;
    //
    //         Vector2 perpendicular1 = r1[i].Perpendicular();
    //         Vector2 perpendicular2 = r2[i].Perpendicular();
    //
    //         Vector2 relativeVelocity = (newVelocity2 + perpendicular2 * newAngularVelocity2) - (newVelocity1 + perpendicular1 * newAngularVelocity1);
    //         Fixed16_16 velocityMagnitude = relativeVelocity.Dot(collisionInfo.Normal);
    //
    //         if (velocityMagnitude > 0) continue;
    //
    //         Fixed16_16 dot1 =  perpendicular1.Dot(collisionInfo.Normal);
    //         Fixed16_16 dot2 =  perpendicular2.Dot(collisionInfo.Normal);
    //
    //         j[i] = -((Fixed16_16(1) + restitution) * velocityMagnitude / (inverseMass1 + inverseMass2 + dot1 * dot1 * inverseInertia1 + dot2 * dot2 * inverseInertia2)) / collisionInfo.ContactCount;
    //         impulses[i] = collisionInfo.Normal * j[i];
    //     }
    //
    //     for(int i = 0; i < collisionInfo.ContactCount; ++i)
    //     {
    //         Vector2 impulse = impulses[i];
    //
    //         newVelocity1 -= impulse * inverseMass1;
    //         newVelocity2 += impulse * inverseMass2;
    //
    //         newAngularVelocity1 -= r1[i].Cross(impulse) * inverseInertia1;
    //         newAngularVelocity2 += r2[i].Cross(impulse) * inverseInertia2;
    //     }
    //
    //     impulses = { };
    //
    //     //Friction
    //     for(int i = 0; i < collisionInfo.ContactCount; ++i)
    //     {
    //         Vector2 perpendicular1 = r1[i].Perpendicular();
    //         Vector2 perpendicular2 = r2[i].Perpendicular();
    //
    //         Vector2 relativeVelocity = (newVelocity2 + perpendicular2 * newAngularVelocity2) - (newVelocity1 + perpendicular1 * newAngularVelocity1);
    //         Vector2 tangent = relativeVelocity - collisionInfo.Normal * relativeVelocity.Dot(collisionInfo.Normal);
    //
    //         if (Vector2::AlmostEqual(tangent, Vector2::Zero())) continue;
    //
    //         tangent = tangent.Normalize();
    //
    //         Fixed16_16 dot1 =  perpendicular1.Dot(tangent);
    //         Fixed16_16 dot2 =  perpendicular2.Dot(tangent);
    //
    //         Fixed16_16 jT = -(relativeVelocity.Dot(tangent) / (inverseMass1 + inverseMass2 + dot1 * dot1 * inverseInertia1 + dot2 * dot2 * inverseInertia2)) / collisionInfo.ContactCount;
    //
    //         Vector2 frictionImpulse;
    //
    //         //Apply coulombs law
    //         if (fpm::abs(jT) <= j[i] * staticFriction)
    //         {
    //             frictionImpulse = tangent * jT;
    //         }
    //         else
    //         {
    //             frictionImpulse = tangent * (-j[i] * dynamicFriction);
    //         }
    //
    //         impulses[i] = frictionImpulse;
    //     }
    //
    //     for(int i = 0; i < collisionInfo.ContactCount; ++i)
    //     {
    //         Vector2 frictionImpulse = impulses[i];
    //
    //         newVelocity1 -= frictionImpulse * inverseMass1;
    //         newVelocity2 += frictionImpulse * inverseMass2;
    //
    //         newAngularVelocity1 -= r1[i].Cross(frictionImpulse) * inverseInertia1;
    //         newAngularVelocity2 += r2[i].Cross(frictionImpulse) * inverseInertia2;
    //     }
    //
    //     if (cacheResults)
    //     {
    //         CollisionResponseInfo responseInfo = CollisionResponseInfo(colliderTransform1.Position, colliderTransform2.Position, newVelocity1, newVelocity2, newAngularVelocity1, newAngularVelocity2);
    //         collisionCache->CacheCollision(currentFrameNumber, check, responseInfo);
    //     }
    //
    //     //Apply velocities
    //     rigidBodyData1.Velocity = newVelocity1;
    //     rigidBodyData2.Velocity = newVelocity2;
    //     rigidBodyData1.AngularVelocity = newAngularVelocity1;
    //     rigidBodyData2.AngularVelocity = newAngularVelocity2;
    // }


private:
    inline Fixed16_16 clamp(Fixed16_16 value, Fixed16_16 min, Fixed16_16 max)
    {
        return fpm::max(min, fpm::min(value, max));
    }

private:
    FrameNumber currentFrameNumber;
    CollisionDetection collisionDetection;

    ComponentCollection<ColliderTransform>* colliderTransformCollection;
    ComponentCollection<RigidBodyData>* rigidBodyDataCollection;
    ComponentCollection<CircleCollider>* circleColliderCollection;
    ComponentCollection<BoxCollider>* boxColliderCollection;
    ComponentCollection<PolygonCollider>* polygonColliderCollection;

    //Caching
    CollisionCache* collisionCache;
    PhysicsCache* physicsCache;

public:
    std::vector<ContactPair> ContactPairs;
    std::vector<CollisionCheckInfo> CollisionChecks;
    //std::vector<CollisionResponseInfo> CachedResponseInfo;
    EntitySet<MAXENTITIES> Entities;
};