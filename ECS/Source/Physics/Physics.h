#pragma once

//ECS & Math
#include "../ECS/ECS.h"
#include "../Math/FixedTypes.h"
#include "../ECS/EntitySet.h" //TODO FIX INCLUDES

#include "PhysicsSettings.h"

//Cache
#include "Cache/SortedCache.h"

#include "Collision/ContactPair.h"
using ImpulseCache = SortedCache<ImpulseData, MaxCollisionCount>;

#include "Collision/CollisionPairData.h"
using CollisionPairCache = SortedCache<CollisionPairData, 3000>;

//Components
#include "PhysicsComponents.h"

using PhysicsComponentManager = ComponentManager<PhysicsComponents>;
static constexpr uint8_t PhysicsComponentCount = PhysicsComponentManager::GetComponentCount();
using PhysicsSignature = std::bitset<PhysicsComponentCount>;

//Systems
#include "PhysicsSystems.h"

using PhysicsSystemManager = SystemManager<PhysicsComponents, PhysicsSystems>;
using PhysicsLayer = Layer<PhysicsComponents, PhysicsSystems>;

//Utility
#include "PhysicsUtils.h"

//Assert trivial types
static_assert(std::is_trivially_default_constructible_v<ImpulseCache>, "Sorted cache needs to be trivial");