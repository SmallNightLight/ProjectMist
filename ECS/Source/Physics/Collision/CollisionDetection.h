#pragma once

class CollisionDetection
{
public:
      explicit  CollisionDetection(ECSWorld* world)
      {
            rigidBodyDataCollection = world->GetComponentCollection<RigidBodyData>();
            circleColliderCollection = world->GetComponentCollection<CircleCollider>();
            boxColliderCollection = world->GetComponentCollection<BoxCollider>();
      }

      static void CollisionCorrection(ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2, const CollisionInfo& resultInfo)
      {
            if (!colliderTransform1.IsDynamic)
            {
                  colliderTransform2.MovePosition(resultInfo.Normal * resultInfo.Depth);
            }
            else if (!colliderTransform2.IsDynamic)
            {
                  colliderTransform1.MovePosition(-resultInfo.Normal * resultInfo.Depth);
            }
            else
            {
                  Vector2 direction = resultInfo.Normal * (resultInfo.Depth / 2);

                  colliderTransform1.MovePosition(-direction);
                  colliderTransform2.MovePosition(direction);
            }
      }

      bool DetectCollisionAndCorrect(Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2, CollisionInfo& resultInfo) const
      {
            //Skip if none of the objects are dynamic
            if (!colliderTransform1.IsDynamic && !colliderTransform2.IsDynamic)
            {
                  return false;
            }

            //Check for different shape types and do the correct collision detection
            if (colliderTransform1.Shape == Circle)
            {
                  if (colliderTransform2.Shape == Circle)
                  {
                        return CircleCircleCollision(entity1, entity2, colliderTransform1, colliderTransform2, resultInfo);
                  }
                  if (colliderTransform2.Shape == Box)
                  {
                        return CircleBoxCollision(entity1, entity2, colliderTransform1, colliderTransform2, false, resultInfo);
                  }
                  if (colliderTransform2.Shape == Convex)
                  {
                  std::cout << "Collision type not defined" << std::endl;
                  }
            }
            else if (colliderTransform1.Shape == Box)
            {
                  if (colliderTransform2.Shape == Circle)
                  {
                        return CircleBoxCollision(entity2, entity1, colliderTransform2, colliderTransform1, true, resultInfo);
                  }
                  if (colliderTransform2.Shape == Box)
                  {
                        return BoxBoxCollision(entity1, entity2, colliderTransform1, colliderTransform2, false, resultInfo);
                  }
                  if (colliderTransform2.Shape == Convex)
                  {
                        std::cout << "Collision type not defined" << std::endl;
                  }
            }
            else if (colliderTransform1.Shape == Convex)
            {
                  if (colliderTransform2.Shape == Circle)
                  {
                        std::cout << "Collision type not defined" << std::endl;
                  }
                  else if (colliderTransform2.Shape == Box)
                  {
                        std::cout << "Collision type not defined" << std::endl;
                  }
                  else if (colliderTransform2.Shape == Convex)
                  {
                        std::cout << "Collision type not defined" << std::endl;
                  }
            }

            return false;
      }

      bool CircleCircleCollision(Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2, CollisionInfo& resultInfo) const
      {
            assert(circleColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Circle) attached");
            assert(circleColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Circle) attached");

            //Get the components
            CircleCollider& circleCollider1 = circleColliderCollection->GetComponent(entity1);
            CircleCollider& circleCollider2 = circleColliderCollection->GetComponent(entity2);

            //First perform an AABB check, to test if the objects are even able to collide
            if (!colliderTransform1.GetAABB(circleCollider1).Overlaps(colliderTransform2.GetAABB(circleCollider2)))
            {
                  return false;
            }

            Fixed16_16 distance = colliderTransform1.Position.Distance(colliderTransform2.Position);
            Fixed16_16 totalRadius = circleCollider1.Radius + circleCollider2.Radius;

            if (distance >= totalRadius)
            {
                  //No collision
                  return false;
            }

            //Detected discrete collision
            resultInfo.Normal = -(colliderTransform1.Position - colliderTransform2.Position).Normalize();
            resultInfo.Depth = totalRadius - distance;
            resultInfo.Entity1 = entity1;
            resultInfo.Entity2 = entity2;
            resultInfo.IsDynamic1 = colliderTransform1.IsDynamic;
            resultInfo.IsDynamic2 = colliderTransform2.IsDynamic;

            CollisionCorrection(colliderTransform1, colliderTransform2, resultInfo);
            GetContact(colliderTransform1, colliderTransform2, circleCollider1, resultInfo);

            return true;
      }

      //SAP - optimized for polygons
      bool BoxBoxCollision(Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2, bool swap, CollisionInfo& resultInfo) const
      {
            assert(boxColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Box) attached");
            assert(boxColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Box) attached");

            //Get the components
            BoxCollider& boxCollider1 = boxColliderCollection->GetComponent(entity1);
            BoxCollider& boxCollider2 = boxColliderCollection->GetComponent(entity2);

            //First perform an AABB check, to test if the objects are even able to collide
            if (!colliderTransform1.GetAABB(boxCollider1).Overlaps(colliderTransform2.GetAABB(boxCollider2)))
            {
                  return false;
            }

            resultInfo.Depth = std::numeric_limits<Fixed16_16>::max();

            std::vector<Vector2> vertices1 = colliderTransform1.GetTransformedVertices(boxCollider1);
            std::vector<Vector2> vertices2 = colliderTransform2.GetTransformedVertices(boxCollider2);

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

            Vector2 center1 = GetCenter(vertices1);
            Vector2 center2 = GetCenter(vertices2);
            Vector2 direction = center2 - center1;

            if (direction.Dot(resultInfo.Normal) < 0)
            {
                  resultInfo.Normal = -resultInfo.Normal;
            }

            resultInfo.Normal *= swap ? -1 : 1;
            resultInfo.Entity1 = entity1;
            resultInfo.Entity2 = entity2;
            resultInfo.IsDynamic1 = colliderTransform1.IsDynamic;
            resultInfo.IsDynamic2 = colliderTransform2.IsDynamic;

            CollisionCorrection(colliderTransform1, colliderTransform2, resultInfo);
            //GetContact(colliderTransform1, colliderTransform2, boxCollider1, resultInfo);

            return true;
      }

      bool CircleBoxCollision(Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2, bool swap, CollisionInfo& resultInfo) const
      {
            assert(circleColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Circle) attached");
            assert(boxColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Box) attached");

            //Get the components
            CircleCollider& circleCollider1 = circleColliderCollection->GetComponent(entity1);
            BoxCollider& boxCollider2 = boxColliderCollection->GetComponent(entity2);

            //First perform an AABB check, to test if the objects are even able to collide
            if (!colliderTransform1.GetAABB(circleCollider1).Overlaps(colliderTransform2.GetAABB(boxCollider2)))
            {
                  //No collision possible
                  return false;
            }

            resultInfo.Depth = std::numeric_limits<Fixed16_16>::max();

            std::vector<Vector2> vertices = colliderTransform2.GetTransformedVertices(boxCollider2);

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

            resultInfo.Normal *= swap ? -1 : 1;

            if (swap)
            {
                  resultInfo.Entity1 = entity2;
                  resultInfo.Entity2 = entity1;
                  resultInfo.IsDynamic1 = colliderTransform2.IsDynamic;
                  resultInfo.IsDynamic2 = colliderTransform1.IsDynamic;

                  CollisionCorrection(colliderTransform2, colliderTransform1, resultInfo);
            }
            else
            {
                  resultInfo.Entity1 = entity1;
                  resultInfo.Entity2 = entity2;
                  resultInfo.IsDynamic1 = colliderTransform1.IsDynamic;
                  resultInfo.IsDynamic2 = colliderTransform2.IsDynamic;

                  CollisionCorrection(colliderTransform1, colliderTransform2, resultInfo);
            }

            //GetContact(colliderTransform1, colliderTransform2, boxCollider2, resultInfo);

            return true;
      }

      //Contact point functions

      static void GetContact(const ColliderTransform& colliderTransform1, const ColliderTransform& colliderTransform2, const CircleCollider& circleCollider1, CollisionInfo& collisionInfo)
      {
            Vector2 direction = (colliderTransform2.Position - colliderTransform1.Position).Normalize();
            collisionInfo.Contact1 = colliderTransform1.Position + direction * circleCollider1.Radius;
            collisionInfo.ContactCount = 1;
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
            Vector2 directionAndRadius = axis * radius;
            Vector2 point1 = center + directionAndRadius;
            Vector2 point2 = center -directionAndRadius;

            min = point1.Dot(axis);
            max = point2.Dot(axis);

            if (min > max)
            {
                  swap(min, max);
            }
      }

      static Vector2 GetCenter(const std::vector<Vector2>& vertices) //TODO: can be avoided
      {
            Vector2 sum;
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
            Vector2 result;
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
      ComponentCollection<BoxCollider>* boxColliderCollection;
};
