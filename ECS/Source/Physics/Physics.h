#pragma once

#include "../ECS/ECS.h"
#include "../Math/FixedTypes.h"
#include "../ECS/EntitySet.h" //TODO FIX INCLUDES
#include "PhysicsSettings.h"

#include "PhysicsComponents.h"

using PhysicsComponentManager = ComponentManager<PhysicsComponents>;
static constexpr uint8_t PhysicsComponentCount = PhysicsComponentManager::GetComponentCount();
using PhysicsSignature = std::bitset<PhysicsComponentCount>;

#include "PhysicsSystems.h"

using PhysicsSystemManager = SystemManager<PhysicsComponents, PhysicsSystems>;
using PhysicsLayer = Layer<PhysicsComponents, PhysicsSystems>;

#include "PhysicsUtils.h"