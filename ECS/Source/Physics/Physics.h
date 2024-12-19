#pragma once

#include "../ECS/Layer.h"
#include "../Math/FixedTypes.h"
#include "../Math/AABB.h"
#include "PhysicsSettings.h"

#include "Collision/CollisionInfo.h"

//Includes - Components
#include "Components/ColliderTransform.h"
#include "Components/RigidBodyData.h"
#include "Components/CircleCollider.h"
#include "Components/BoxCollider.h"
#include "Components/PolygonCollider.h"
#include "Components/ColliderRenderData.h"
#include "Components/Movable.h"

//Collision Detection
#include "Collision/CollisionDetection.h"

//Includes - Systems
#include "Systems/RigidBody.h"

#include "Systems/CircleColliderRenderer.h"
#include "Systems/BoxColliderRenderer.h"
#include "Systems/PolygonColliderRenderer.h"

#include "Systems/MovingSystem.h"

#include "PhysicsUtils.h"