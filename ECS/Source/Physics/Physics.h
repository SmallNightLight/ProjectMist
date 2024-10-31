#pragma once

#include "../Base/ECSManager.h"
#include "../Math/FixedTypes.h"
#include "PhysicsSettings.h"

#include "Collision/CollisionInfo.h"

//Includes - Components
#include "Components/ColliderTransform.h"
#include "Components/RigidBodyData.h"
#include "Components/BoxCollider.h"
#include "Components/CircleCollider.h"
#include "Components/ColliderRenderData.h"
#include "Components/Movable.h"

//Collision Detection
#include "Collision/CollisionDetection.h"

//Includes - Systems
#include "Systems/RigidBody.h"

#include "Systems/BoxColliderRenderer.h"
#include "Systems/CircleColliderRenderer.h"

#include "Systems/MovingSystem.h"
