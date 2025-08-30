#pragma once

//ECS & Math
#include "../ECS/ECS.h"
#include "../Math/FixedTypes.h"
#include "../ECS/EntitySet.h" //TODO FIX INCLUDES

#include "PhysicsSettings.h"

//Cache
#include "Cache/SortedMap.h"
#include "Cache/SortedDoubleSet.h"
#include "Cache/SortedDoubleMap.h"
#include "Cache/SortedMapArray.h"

#include "Collision/ContactPair.h"
using ImpulseCache = SortedMap<EntityPair, ImpulseData, MaxCollisionCount>;
using CollisionPairCache = SortedDoubleSet<EntityPair, MaxCollisionCount>;
using CollisionResultCache = SortedDoubleMap<EntityPair, ContactPair, MaxCollisionCount>;
using SolverCache = SortedMapArray<EntityPair, VelocityData, PhysicsIterations, MaxCollisionCount>;

//Components
#include "PhysicsComponents.h"

using PhysicsComponentManager = ComponentManager<PhysicsComponents>;
static constexpr uint8_t PhysicsComponentCount = PhysicsComponentManager::GetComponentCount();
using PhysicsSignature = std::bitset<PhysicsComponentCount>;

//Cache
#include "Cache/ComponentCollectionCache.h"
using TransformCache = ComponentCollectionCache<Transform>;
using RigidBodyDataCache = ComponentCollectionCache<RigidBodyData>;
#include "Collision/CollisionCache.h"
#include "Collision/PhysicsCache.h"

//Systems
#include "PhysicsSystems.h"

using PhysicsSystemManager = SystemManager<PhysicsComponents, PhysicsSystems>;
using PhysicsLayer = Layer<PhysicsComponents, PhysicsSystems>;

//Utility
#include "PhysicsUtils.h"

//Assert trivial types
static_assert(std::is_trivially_default_constructible_v<ImpulseCache>, "Sorted cache needs to be trivial");