#pragma once

//ECS & Math
#include "../ECS/ECS.h"
#include "../Math/FixedTypes.h"
#include "../ECS/EntitySet.h" //TODO FIX INCLUDES

#include "PhysicsSettings.h"

//Cache
#include "Cache/SortedCache.h"
#include "Cache/SortedDoubleCache.h"
#include "Cache/SortedDoubleMap.h"

#include "Collision/ContactPair.h"
using ImpulseCache = SortedCache<EntityPair, ImpulseData, MaxCollisionCount>;
using CollisionPairCache = SortedDoubleCache<EntityPair, 3000>;
using CollisionResultCache = SortedDoubleMap<EntityPair, ContactPair, MaxCollisionCount>;

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