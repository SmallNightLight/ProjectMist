#pragma once

#include "../Base/ECSManager.h"
#include "../Math/FixedTypes.h"
#include "PhysicsSettings.h"

//Includes - Components
#include "Components/ColliderTransform.h"
#include "Components/RigidBodyData.h"
#include "Components/BoxCollider.h"
#include "Components/CircleCollider.h"
#include "Components/ColliderRenderData.h"

//Includes - Systems
#include "Systems/RigidBody.h"

#include "Systems/BoxColliderRenderer.h"
#include "Systems/CircleColliderRenderer.h"