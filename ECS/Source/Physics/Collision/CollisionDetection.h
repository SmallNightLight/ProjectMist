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

      //SAP - optimized for polygons
      bool BoxBoxCollisionDetection(Entity entity1, Entity entity2, const ColliderTransform& colliderTransform1, const ColliderTransform& colliderTransform2, CollisionInfo& resultInfo)
      {
            assert(boxColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Box) attached");
            assert(boxColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Box) attached");

            resultInfo.Normal = Vector2::Zero();
            resultInfo.Depth = std::numeric_limits<Fixed16_16>::max();

            //Get the components
            RigidBodyData& rigidBodyData1 = rigidBodyDataCollection->GetComponent(entity1);
            RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(entity2);

            BoxCollider& boxCollider1 = boxColliderCollection->GetComponent(entity1);
            BoxCollider& boxCollider2 = boxColliderCollection->GetComponent(entity2);

            std::vector<Vector2> vertices1 = GetTransformedVertices(colliderTransform1, boxCollider1);
            std::vector<Vector2> vertices2 = GetTransformedVertices(colliderTransform2, boxCollider2);

            for(int i = 0; i < vertices1.size(); ++i)
            {
                  Vector2 v1 = vertices1[i];
                  Vector2 v2 = vertices1[(i + 1) % vertices1.size()];

                  Vector2 edge = v2 - v1;
                  Vector2 axis = edge.Perpendicular().Normalize();

                  Fixed16_16 min1, max1, min2, max2;
                  ProjectVertices(vertices1, axis, min1, max1);
                  ProjectVertices(vertices2, axis, min2, max2);

                  if (min1 >= max2 || min2 >= max1)
                  {
                        //Separation detected - No collision
                        return false;
                  }

                  Fixed16_16 axisDepth = fpm::min(max2 - min1, max1 - min2);
                  if (axisDepth < resultInfo.Depth)
                  {
                        resultInfo.Depth = axisDepth;
                        resultInfo.Normal = axis;
                  }
            }

            for(int i = 0; i < vertices2.size(); ++i)
            {
                  Vector2 v1 = vertices2[i];
                  Vector2 v2 = vertices2[(i + 1) % vertices2.size()];

                  Vector2 edge = v2 - v1;
                  Vector2 axis = edge.Perpendicular().Normalize();

                  Fixed16_16 min1, max1, min2, max2;
                  ProjectVertices(vertices1, axis, min1, max1);
                  ProjectVertices(vertices2, axis, min2, max2);

                  if (min1 >= max2 || min2 >= max1)
                  {
                        //Separation detected - No collision
                        return false;
                  }

                  Fixed16_16 axisDepth = fpm::min(max2 - min1, max1 - min2);
                  if (axisDepth < resultInfo.Depth)
                  {
                        resultInfo.Depth = axisDepth;
                        resultInfo.Normal = axis;
                  }
            }

            //Detected a collision
            //resultInfo.Depth /= resultInfo.Normal.Magnitude();
            //resultInfo.Normal = resultInfo.Normal.Normalize();

            Vector2 center1 = GetCenter(vertices1);
            Vector2 center2 = GetCenter(vertices2);
            Vector2 direction = center2 - center1;

            if (direction.Dot(resultInfo.Normal) < 0)
            {
                  resultInfo.Normal = -resultInfo.Normal;
            }

            return true;
      }

      bool CircleBoxCollisionDetection(Entity entity1, Entity entity2, const ColliderTransform& colliderTransform1, const ColliderTransform& colliderTransform2, CollisionInfo& resultInfo)
      {
            assert(circleColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Circle) attached");
            assert(boxColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Box) attached");

            resultInfo.Normal = Vector2::Zero();
            resultInfo.Depth = std::numeric_limits<Fixed16_16>::max();

            //Get the components
            RigidBodyData& rigidBodyData1 = rigidBodyDataCollection->GetComponent(entity1);
            RigidBodyData& rigidBodyData2 = rigidBodyDataCollection->GetComponent(entity2);

            CircleCollider& circleCollider1 = circleColliderCollection->GetComponent(entity1);
            BoxCollider& boxCollider2 = boxColliderCollection->GetComponent(entity2);

            std::vector<Vector2> vertices = GetTransformedVertices(colliderTransform2, boxCollider2);

            for(int i = 0; i < vertices.size(); ++i)
            {
                  Vector2 v1 = vertices[i];
                  Vector2 v2 = vertices[(i + 1) % vertices.size()];

                  Vector2 edge = v2 - v1;
                  Vector2 axis = edge.Perpendicular().Normalize();

                  Fixed16_16 min1, max1, min2, max2;
                  ProjectVertices(vertices, axis, min1, max1);
                  ProjectCircle(colliderTransform1.Position, circleCollider1.Radius, axis, min2, max2);

                  if (min1 >= max2 || min2 >= max1)
                  {
                        //Separation detected - No collision
                        return false;
                  }

                  Fixed16_16 axisDepth = fpm::min(max2 - min1, max1 - min2);
                  if (axisDepth < resultInfo.Depth)
                  {
                        resultInfo.Depth = axisDepth;
                        resultInfo.Normal = axis;
                  }
            }

            Vector2 closestVertexToCircle = GetClosestPointToCircle(colliderTransform1.Position, vertices);
            Vector2 axis = (closestVertexToCircle - colliderTransform1.Position).Normalize();

            //EcsManager.GetComponent<ColliderRenderData>(entity2).p = colliderTransform1.Position;

            Fixed16_16 min1, max1, min2, max2;
            ProjectVertices(vertices, axis, min1, max1);
            ProjectCircle(colliderTransform1.Position, circleCollider1.Radius, axis, min2, max2);

            if (min1 >= max2 || min2 >= max1)
            {
                  //Separation detected - No collision
                  return false;
            }

            Fixed16_16 axisDepth = fpm::min(max2 - min1, max1 - min2);
            if (axisDepth < resultInfo.Depth)
            {
                  resultInfo.Depth = axisDepth;
                  resultInfo.Normal = axis;
            }

            Vector2 polygonCenter = GetCenter(vertices);
            Vector2 direction = polygonCenter - colliderTransform1.Position;

            if (direction.Dot(resultInfo.Normal) < 0)
            {
                  resultInfo.Normal = -resultInfo.Normal;
            }

            return true;
      }

public:
      static std::vector<Vector2> GetTransformedVertices(const ColliderTransform& colliderTransform, BoxCollider& boxCollider)
      {
            if (boxCollider.TransformUpdateRequired)
            {
                  for (int i = 0; i < boxCollider.TransformedVertices.size(); ++i)
                  {
                        boxCollider.TransformedVertices[i] = colliderTransform.Transform(boxCollider.Vertices[i]);
                  }

                  boxCollider.TransformUpdateRequired = false;
            }

            return boxCollider.TransformedVertices;
      }

private:
      static void ProjectVertices(const std::vector<Vector2>& vertices, const Vector2& axis, Fixed16_16& min, Fixed16_16& max)
      {
            min = std::numeric_limits<Fixed16_16>::max();
            max = std::numeric_limits<Fixed16_16>::min();

            for (Vector2 vertex : vertices)
            {
                  Fixed16_16 projection = vertex.Dot(axis);

                  if (projection < min)
                        min = projection;

                  if(projection > max)
                        max = projection;
            }
      }

      static void ProjectCircle(const Vector2& center, const Fixed16_16& radius, const Vector2& axis, Fixed16_16& min, Fixed16_16& max)
      {
            Vector2 directonAndRadius = axis * radius;
            Vector2 point1 = center + directonAndRadius;
            Vector2 point2 = center -directonAndRadius;

            min = point1.Dot(axis);
            max = point2.Dot(axis);

            if (min > max)
            {
                  swap(min, max);
            }
      }

      static Vector2 GetCenter(const std::vector<Vector2>& vertices)
      {
            Vector2 sum = Vector2::Zero();
            short vertexCount = 0;

            for (auto& vertex : vertices)
            {
                  ++vertexCount;
                  sum += vertex;
            }

            return sum / vertexCount;
      }

      static Vector2 GetClosestPointToCircle(Vector2 center, const std::vector<Vector2>& vertices)
      {
            Vector2 result = Vector2::Zero();
            Fixed16_16 minDistance = std::numeric_limits<Fixed16_16>::max();

            for (auto& vertex : vertices)
            {
                  Fixed16_16 distance = vertex.Distance(center);

                  if (distance < minDistance)
                  {
                        minDistance = distance;
                        result = vertex;
                  }
            }

            return result;
      }

private:
      template <typename T>
      inline static void swap(T& x, T& y)
      {
            T temp = x;
            x = y;
            y = temp;
      }

private:
      ComponentCollection<RigidBodyData>* rigidBodyDataCollection;
      ComponentCollection<CircleCollider>* circleColliderCollection;
      ComponentCollection<BoxCollider>* boxColliderCollection; //TODO: Can't make multiple ECS worlds with only one static collection
};
