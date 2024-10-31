#pragma once

class CollisionDetection
{
public:
      CollisionDetection()
      {
            rigidBodyDataCollection = EcsManager.GetComponentCollection<RigidBodyData>();
            circleColliderCollection = EcsManager.GetComponentCollection<CircleCollider>();
            boxColliderCollection = EcsManager.GetComponentCollection<BoxCollider>();
      }

      bool CircleCircleCollision(Entity entity1, Entity entity2, const ColliderTransform& colliderTransform1, const ColliderTransform& colliderTransform2, CollisionInfo& resultInfo)
      {
            assert(circleColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Circle) attached");
            assert(circleColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Circle) attached");

            //Get the components
            RigidBodyData& rigidBodyData1 = rigidBodyDataCollection->GetComponent(entity1);
            RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(entity2);

            CircleCollider& circleCollider1 = circleColliderCollection->GetComponent(entity1);
            CircleCollider& circleCollider2 = circleColliderCollection->GetComponent(entity2);

            Fixed16_16 distance = colliderTransform1.Position.Distance(colliderTransform2.Position);
            Fixed16_16 totalRadius = circleCollider1.Radius + circleCollider2.Radius;

            if (distance >= totalRadius)
            {
                  //No collision
                  return false;
            }

            //Detected discrete collision
            resultInfo.Normal = (colliderTransform1.Position - colliderTransform2.Position).Normalize();
            resultInfo.Depth = totalRadius - distance;

            return true;
      }

private:
      ComponentCollection<RigidBodyData>* rigidBodyDataCollection;
      ComponentCollection<CircleCollider>* circleColliderCollection;
      ComponentCollection<BoxCollider>* boxColliderCollection; //TODO: Can't make multiple ECS worlds with only one static collection
};
