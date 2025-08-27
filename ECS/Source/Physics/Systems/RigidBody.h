#pragma once

#include "../../ECS/ECS.h"
#include "../PhysicsSettings.h"
#include "../Collision/CollisionDetection.h"

#include <vector>

class RigidBody
{
public:
    using RequiredComponents = ComponentList<Transform, TransformMeta, RigidBodyData>;

    explicit RigidBody(PhysicsComponentManager& componentManager) : collisionDetection(componentManager) //TODO: Static objects should not need to have a rigidBody
    {
        transformCollection = componentManager.GetComponentCollection<Transform>();
        transformMetaCollection = componentManager.GetComponentCollection<TransformMeta>();
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

    void HandleCollisions(FrameNumber frame)
    {
        assert(collisionCache && "CollisionCache is null");

        //Update & Validate collision cache
        bool useCache = collisionCache->UpdateFrame(frame);

        ContactPairs.clear();
        collisionCache->Flip();

        //Setup transform from cache
        SetupEntityTransforms(useCache);
        collisionCache->CacheTransformCollection(transformCollection);
        collisionCache->CacheRigidBodyDataCollection(rigidBodyDataCollection);

        for (Entity* it1 = Entities.begin(); it1 != Entities.end(); ++it1)
        {
            const Entity& entity1 = *it1;
            Transform& transform1 = transformCollection->GetComponent(entity1);

            //Detect collisions
            for (Entity* it2 = std::next(it1); it2 != Entities.end(); ++it2)
            {
                const Entity& entity2 = *it2;

                //Entity pairs should be ordered

                Transform& transform2 = transformCollection->GetComponent(entity2);
                EntityPair entityPair = EntityPair::Make(entity1, entity2);

                //Check if collision already occurred in the past
                if (useCache && !transform1.Changed && !transform2.Changed)
                {
                    //The collision has already happened before (same position and rotation)
                    bool collision = collisionCache->AdvancePairCache(entityPair);

                    if (!collision) continue;

                    //Get collision response data
                    ContactPair cachedContactPair;
                    if (collisionCache->AdvanceCollisionCache(entityPair, cachedContactPair))
                    {
                        ContactPairs.emplace_back(cachedContactPair);
                    }
                    else
                    {
                        assert(false && "Should not be possible");
                    }

                    continue;
                }

                RigidBodyData& rigidBodyData1 = rigidBodyDataCollection->GetComponent(entity1);
                RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(entity2);
                TransformMeta& transformMeta1 = transformMetaCollection->GetComponent(entity1);
                TransformMeta& transformMeta2 = transformMetaCollection->GetComponent(entity2);

                ContactPair contactPair = ContactPair();    //Value initialization to give the impulses zero values
                if (collisionDetection.DetectCollision(entity1, entity2, transform1, transform2, transformMeta1, transformMeta2, contactPair))
                {
                    SetupContactPair(contactPair, rigidBodyData1, rigidBodyData2);
                    ContactPairs.emplace_back(contactPair);
                    collisionCache->CacheCollisionPair(entityPair);
                    collisionCache->CacheCollision(contactPair);
                }

                //In case of no collision, it is not cached as the fact that it is not present in the cache means that there is not collision.
                //Only possible since we first check if the entity has changed during rollback
            }
        }
    }

    void SetupEntityTransforms(bool useCache) //optimize inline in the handlecol? todo divide into two bools for both
    {
        if (useCache)
        {
            Transform cachedTransform;
            for (const Entity& entity : Entities)
            {
                if (collisionCache->TryGetTransform(entity, cachedTransform))
                {
                    Transform& transform = transformCollection->GetComponent(entity);
                    transform.Changed = transform.Key != cachedTransform.Key;
                }
                else
                {
                    transformCollection->GetComponent(entity).Changed = true;
                }
            }

            //Not needed as it only stores contact pairs currently
            // RigidBodyData cachedRigidBodyData;
            // for (const Entity& entity : Entities)
            // {
            //     if (collisionCache->TryGetRigidBodyData(entity, cachedRigidBodyData))
            //     {
            //         RigidBodyData& rigidBodyData = rigidBodyDataCollection->GetComponent(entity);
            //         rigidBodyData.Changed = rigidBodyData.Key != cachedRigidBodyData.Key;
            //     }
            //     else
            //     {
            //         rigidBodyDataCollection->GetComponent(entity).Changed = true;
            //     }
            // }
        }
        else
        {
            for (const Entity& entity : Entities)
            {
                transformCollection->GetComponent(entity).Changed = true;
            }
        }
    }

    inline void SetupContactPair(ContactPair& contactPair, const RigidBodyData& rigidBodyData1, const RigidBodyData& rigidBodyData2) const
    {
        if (contactPair.ContactCount == 0) return; //todo remove actually gets called

        //Apply previous impulses
        if (WarmStarting)
        {
            EntityPair entityPair = EntityPair::Make(contactPair.Entity1, contactPair.Entity2);
            ImpulseData lastImpulseData;

            ImpulseData newImpulseData;
            newImpulseData.EntityKey = entityPair;
            newImpulseData.ContactCount = contactPair.ContactCount;

            if (physicsCache->TryGetImpulseData(entityPair, lastImpulseData))
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

        //Precompute tangents on all contacts
        for (int i = 0; i < contactPair.ContactCount; ++i)
        {
            Contact& contact = contactPair.Contacts[i];

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
        }
    }

    void IntegrateForces(Fixed16_16 deltaTime)
    {
        for (const Entity& entity : Entities)
        {
            TransformMeta& transformMeta = transformMetaCollection->GetComponent(entity);

            if (transformMeta.IsStatic) continue;

            RigidBodyData& rigidBodyData = rigidBodyDataCollection->GetComponent(entity);

            rigidBodyData.Base.Velocity += (Gravity + rigidBodyData.Force * rigidBodyData.InverseMass) * deltaTime;
            //rigidBodyData.AngularVelocity += deltaTime * rigidBodyData.InverseInertia * rigidBodyData.Torque; //todo
        }
    }

    static constexpr bool WarmStarting = true;

    void SetupContacts()
    {
        if (WarmStarting)
        {
            for (ContactPair& contactPair : ContactPairs)
            {
                RigidBodyData& rigidBodyData1 = rigidBodyDataCollection->GetComponent(contactPair.Entity1);
                RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(contactPair.Entity2);

                for (int i = 0; i < contactPair.ContactCount; ++i)
                {
                    Contact& contact = contactPair.Contacts[i];
                    Vector2 tangent = contactPair.Normal.Perpendicular(); //todo already calculated before?

                    //Apply normal + friction impulse
                    Vector2 P = contactPair.Normal * contact.LastImpulse.Pn + tangent * contact.LastImpulse.Pt;

                    rigidBodyData1.Base.Velocity -= P * rigidBodyData1.InverseMass;
                    rigidBodyData1.Base.AngularVelocity -= rigidBodyData1.InverseInertia * contact.R1.Cross(P);

                    rigidBodyData2.Base.Velocity += P * rigidBodyData2.InverseMass;
                    rigidBodyData2.Base.AngularVelocity += rigidBodyData2.InverseInertia * contact.R2.Cross(P);
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
                Vector2 dv = rigidBodyData2.Base.Velocity + contact.R2.CrossI(rigidBodyData2.Base.AngularVelocity) - rigidBodyData1.Base.Velocity - contact.R1.CrossI(rigidBodyData1.Base.AngularVelocity);

                //Compute normal impulse
                Fixed16_16 vn = dv.Dot(contactPair.Normal);

                Fixed16_16 dPn = contact.MassNormal * -vn;
                if (WarmStarting)
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

                rigidBodyData1.Base.Velocity -= Pn * rigidBodyData1.InverseMass;
                rigidBodyData1.Base.AngularVelocity -= rigidBodyData1.InverseInertia * contact.R1.Cross(Pn);

                rigidBodyData2.Base.Velocity += Pn * rigidBodyData2.InverseMass;
                rigidBodyData2.Base.AngularVelocity += rigidBodyData2.InverseInertia * contact.R2.Cross(Pn);

                //Relative velocity at contact
                dv = rigidBodyData2.Base.Velocity + contact.R2.CrossI(rigidBodyData2.Base.AngularVelocity) - rigidBodyData1.Base.Velocity - contact.R1.CrossI(rigidBodyData1.Base.AngularVelocity);

                Vector2 tangent = contactPair.Normal.Perpendicular();
                Fixed16_16 vt = dv.Dot(tangent);
                Fixed16_16 dPt = contact.MassTangent * -vt;

                if (WarmStarting)
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

                rigidBodyData1.Base.Velocity -= Pt * rigidBodyData1.InverseMass;
                rigidBodyData1.Base.AngularVelocity -= rigidBodyData1.InverseInertia * contact.R1.Cross(Pt);

                rigidBodyData2.Base.Velocity += Pt * rigidBodyData2.InverseMass;
                rigidBodyData2.Base.AngularVelocity += rigidBodyData2.InverseInertia * contact.R2.Cross(Pt);
            }
        }
    }

    void IntegrateVelocities(Fixed16_16 deltaTime)
    {
        for (const Entity& entity : Entities)
        {
            TransformMeta& transformMeta = transformMetaCollection->GetComponent(entity);

            if (transformMeta.IsStatic) continue;

            Transform& transform = transformCollection->GetComponent(entity);
            RigidBodyData& rigidBodyData = rigidBodyDataCollection->GetComponent(entity);

            transform.MovePosition(rigidBodyData.Base.Velocity * deltaTime);
            transform.Rotate(rigidBodyData.Base.AngularVelocity * deltaTime);

            rigidBodyData.Force = Vector2(0, 0);
            //rigidBodyData.Torque = Fixed16_16(0); //todo
        }
    }

    void IntegratePositions()
    {
        physicsCache->ResetImpulses();

        for (ContactPair& contactPair : ContactPairs)
        {
            TransformMeta& transformMeta1 = transformMetaCollection->GetComponent(contactPair.Entity1);     //todo the static check is not worth the access also above
            TransformMeta& transformMeta2 = transformMetaCollection->GetComponent(contactPair.Entity2);
            Transform& transform1 = transformCollection->GetComponent(contactPair.Entity1);
            Transform& transform2 = transformCollection->GetComponent(contactPair.Entity2);
            RigidBodyData& rigidBodyData1 = rigidBodyDataCollection->GetComponent(contactPair.Entity1);
            RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(contactPair.Entity2);

            for (uint8_t i = 0; i < contactPair.ContactCount; ++i)
            {
                Contact& contact = contactPair.Contacts[i];

                //Position correction
                constexpr Fixed16_16 steeringConstant = Fixed16_16(0, 5);
                constexpr Fixed16_16 maxCorrection = -Fixed16_16(5);
                constexpr Fixed16_16 slop = Fixed16_16(1) / Fixed16_16(100);

                Fixed16_16 steeringForce = clamp(steeringConstant * (contact.Separation + slop), maxCorrection, Fixed16_16(0));
                Vector2 impulse = contactPair.Normal * (-steeringForce * contact.MassNormal);

                if (!transformMeta1.IsStatic)
                {
                    transform1.MovePosition(-impulse * rigidBodyData1.InverseMass);
                    Vector2 r1 = contact.Position - transform1.Base.Position;
                    transform1.Rotate(-r1.Cross(impulse) * rigidBodyData1.InverseInertia);
                }

                if (!transformMeta2.IsStatic)
                {
                    transform2.MovePosition(impulse * rigidBodyData2.InverseMass);
                    Vector2 r2 = contact.Position - transform2.Base.Position;
                    transform2.Rotate(r2.Cross(impulse) * rigidBodyData2.InverseInertia);
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

            physicsCache->CacheImpulseData(newImpulses);
        }
    }

private:
    inline Fixed16_16 clamp(Fixed16_16 value, Fixed16_16 min, Fixed16_16 max)
    {
        return fpm::max(min, fpm::min(value, max));
    }

private:
    CollisionDetection collisionDetection;

    ComponentCollection<Transform>* transformCollection;
    ComponentCollection<TransformMeta>* transformMetaCollection;
    ComponentCollection<RigidBodyData>* rigidBodyDataCollection;
    ComponentCollection<CircleCollider>* circleColliderCollection;
    ComponentCollection<BoxCollider>* boxColliderCollection;
    ComponentCollection<PolygonCollider>* polygonColliderCollection;

    //Caching
    CollisionCache* collisionCache;
    PhysicsCache* physicsCache;

public:
    std::vector<ContactPair> ContactPairs;
    //std::vector<CollisionResponseInfo> CachedResponseInfo;
    EntitySet<MAXENTITIES> Entities;
};