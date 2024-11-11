#pragma once

class CollisionDetection
{
public:
      explicit  CollisionDetection(Layer* world)
      {
            rigidBodyDataCollection = world->GetComponentCollection<RigidBodyData>();
            circleColliderCollection = world->GetComponentCollection<CircleCollider>();
            boxColliderCollection = world->GetComponentCollection<BoxCollider>();
            polygonColliderCollection = world->GetComponentCollection<PolygonCollider>();
      }

      static void CollisionCorrection(const CollisionInfo& resultInfo, bool swap, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2)
      {
            if (swap)
            {
                  if (!colliderTransform2.IsDynamic)
                  {
                        colliderTransform1.MovePosition(resultInfo.Normal * resultInfo.Depth);
                  }
                  else if (!colliderTransform1.IsDynamic)
                  {
                        colliderTransform2.MovePosition(-resultInfo.Normal * resultInfo.Depth);
                  }
                  else
                  {
                        Vector2 direction = resultInfo.Normal * (resultInfo.Depth / 2);

                        colliderTransform2.MovePosition(-direction);
                        colliderTransform1.MovePosition(direction);
                  }
            }
            else
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
                        return CircleCircleCollision(resultInfo, entity1, entity2, colliderTransform1, colliderTransform2);
                  }
                  if (colliderTransform2.Shape == Box)
                  {
                        return CircleBoxCollision(resultInfo, false, entity1, entity2, colliderTransform1, colliderTransform2);
                  }
                  if (colliderTransform2.Shape == Polygon)
                  {
                        return CirclePolygonCollision(resultInfo, false, entity1, entity2, colliderTransform1, colliderTransform2);
                  }
            }
            else if (colliderTransform1.Shape == Box)
            {
                  if (colliderTransform2.Shape == Circle)
                  {
                        return CircleBoxCollision(resultInfo, true, entity2, entity1, colliderTransform2, colliderTransform1);
                  }
                  if (colliderTransform2.Shape == Box)
                  {
                        return BoxBoxCollision(resultInfo, entity1, entity2, colliderTransform1, colliderTransform2);
                  }
                  if (colliderTransform2.Shape == Polygon)
                  {
                        return BoxPolygonCollision(resultInfo, false, entity1, entity2, colliderTransform1, colliderTransform2);
                  }
            }
            else if (colliderTransform1.Shape == Polygon)
            {
                  if (colliderTransform2.Shape == Circle)
                  {
                        return CirclePolygonCollision(resultInfo, true, entity2, entity1, colliderTransform2, colliderTransform1);
                  }
                  if (colliderTransform2.Shape == Box)
                  {
                        return BoxPolygonCollision(resultInfo, true, entity2, entity1, colliderTransform2, colliderTransform1);
                  }
                  if (colliderTransform2.Shape == Polygon)
                  {
                        return PolygonPolygonCollision(resultInfo, entity1, entity2, colliderTransform1, colliderTransform2);
                  }
            }

            return false;
      }

      bool CircleCircleCollision(CollisionInfo& resultInfo, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2) const
      {
            assert(circleColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Circle) attached");
            assert(circleColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Circle) attached");

            //Get the components
            CircleCollider& circleCollider1 = circleColliderCollection->GetComponent(entity1);
            CircleCollider& circleCollider2 = circleColliderCollection->GetComponent(entity2);

            //First perform an AABB check, to test if the objects are even able to collide
            if (!colliderTransform1.GetAABB(circleCollider1.Radius).Overlaps(colliderTransform2.GetAABB(circleCollider2.Radius)))
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

            CollisionCorrection(resultInfo, false, colliderTransform1, colliderTransform2);
            GetContactCircleCircle(resultInfo, colliderTransform1.Position, colliderTransform2.Position, circleCollider1.Radius);

            return true;
      }

      bool CircleBoxCollision(CollisionInfo& resultInfo, bool swap, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2) const
      {
            assert(circleColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Circle) attached");
            assert(boxColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Box) attached");

            //Get the components
            CircleCollider& circleCollider1 = circleColliderCollection->GetComponent(entity1);
            BoxCollider& boxCollider2 = boxColliderCollection->GetComponent(entity2);

            const std::vector<Vector2>& vertices = colliderTransform2.GetTransformedVertices(boxCollider2.TransformedVertices, boxCollider2.Vertices);

            return CircleConvexCollision(resultInfo, swap, entity1, entity2, colliderTransform1, colliderTransform2, circleCollider1.Radius, vertices);
      }

      bool CirclePolygonCollision(CollisionInfo& resultInfo, bool swap, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2) const
      {
            assert(circleColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Circle) attached");
            assert(polygonColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Polygon) attached");

            //Get the components
            CircleCollider& circleCollider1 = circleColliderCollection->GetComponent(entity1);
            PolygonCollider& polygonCollider2 = polygonColliderCollection->GetComponent(entity2);

            const std::vector<Vector2>& vertices = colliderTransform2.GetTransformedVertices(polygonCollider2.TransformedVertices, polygonCollider2.Vertices);

            return CircleConvexCollision(resultInfo, swap, entity1, entity2, colliderTransform1, colliderTransform2, circleCollider1.Radius, vertices);
      }

      bool BoxBoxCollision(CollisionInfo& resultInfo, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2) const
      {
            assert(boxColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Box) attached");
            assert(boxColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Box) attached");

            //Get the components
            BoxCollider& boxCollider1 = boxColliderCollection->GetComponent(entity1);
            BoxCollider& boxCollider2 = boxColliderCollection->GetComponent(entity2);

            const std::vector<Vector2>& vertices1 = colliderTransform1.GetTransformedVertices(boxCollider1.TransformedVertices, boxCollider1.Vertices);
            const std::vector<Vector2>& vertices2 = colliderTransform2.GetTransformedVertices(boxCollider2.TransformedVertices, boxCollider2.Vertices);

            return ConvexConvexCollision(resultInfo, false, entity1, entity2, colliderTransform1, colliderTransform2, vertices1, vertices2);
      }

      bool BoxPolygonCollision(CollisionInfo& resultInfo, bool swap, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2) const
      {
            assert(boxColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Box) attached");
            assert(polygonColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Polygon) attached");

            //Get the components
            BoxCollider& boxCollider1 = boxColliderCollection->GetComponent(entity1);
            PolygonCollider& polygonCollider2 = polygonColliderCollection->GetComponent(entity2);

            const std::vector<Vector2>& vertices1 = colliderTransform1.GetTransformedVertices(boxCollider1.TransformedVertices, boxCollider1.Vertices);
            const std::vector<Vector2>& vertices2 = colliderTransform2.GetTransformedVertices(polygonCollider2.TransformedVertices, polygonCollider2.Vertices);

            return ConvexConvexCollision(resultInfo, swap, entity1, entity2, colliderTransform1, colliderTransform2, vertices1, vertices2);
      }

      bool PolygonPolygonCollision(CollisionInfo& resultInfo, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2) const
      {
            assert(polygonColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Polygon) attached");
            assert(polygonColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Polygon) attached");

            //Get the components
            PolygonCollider& polygonCollider1 = polygonColliderCollection->GetComponent(entity1);
            PolygonCollider& polygonCollider2 = polygonColliderCollection->GetComponent(entity2);

            const std::vector<Vector2>& vertices1 = colliderTransform1.GetTransformedVertices(polygonCollider1.TransformedVertices, polygonCollider1.Vertices);
            const std::vector<Vector2>& vertices2 = colliderTransform2.GetTransformedVertices(polygonCollider2.TransformedVertices, polygonCollider2.Vertices);

            return ConvexConvexCollision(resultInfo, false, entity1, entity2, colliderTransform1, colliderTransform2, vertices1, vertices2);
      }

      static bool CircleConvexCollision(CollisionInfo& resultInfo, bool swap, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2, const Fixed16_16& circleRadius, const std::vector<Vector2>& vertices)
      {
            //First perform an AABB check, to test if the objects are even able to collide
            if (!colliderTransform1.GetAABB(circleRadius).Overlaps(colliderTransform2.GetAABBFromTransformed(vertices)))
            {
                  //No collision possible
                  return false;
            }

            resultInfo.Depth = std::numeric_limits<Fixed16_16>::max();

            for(int i = 0; i < vertices.size(); ++i)
            {
                  Vector2 v1 = vertices[i];
                  Vector2 v2 = vertices[(i + 1) % vertices.size()];

                  Vector2 edge = v2 - v1;
                  Vector2 axis = edge.Perpendicular().Normalize();

                  if (CheckCircleAxisSeparation(resultInfo, vertices, colliderTransform1.Position, circleRadius, axis)) return false;
            }

            Vector2 closestVertexToCircle = GetClosestPointToCircle(colliderTransform1.Position, vertices);
            Vector2 axis = (closestVertexToCircle - colliderTransform1.Position).Normalize();

            if (CheckCircleAxisSeparation(resultInfo, vertices, colliderTransform1.Position, circleRadius, axis)) return false;

            FinalizeCollisionInfo(resultInfo, swap, colliderTransform1.Position, GetCenter(vertices), entity1, entity2, colliderTransform1.IsDynamic, colliderTransform2.IsDynamic);
            CollisionCorrection(resultInfo, swap, colliderTransform1, colliderTransform2);
            GetContactCircleConvex(resultInfo, colliderTransform1.Position, vertices);

            return true;
      }

      static bool ConvexConvexCollision(CollisionInfo& resultInfo, bool swap, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2, const std::vector<Vector2>& vertices1, const std::vector<Vector2>& vertices2 )
      {
            //First perform an AABB check, to test if the objects are even able to collide
            if (!colliderTransform1.GetAABBFromTransformed(vertices1).Overlaps(colliderTransform2.GetAABBFromTransformed(vertices2)))
            {
                  return false;
            }

            resultInfo.Depth = std::numeric_limits<Fixed16_16>::max();

            if (CheckAxisSeparation(resultInfo, vertices1, vertices2)) return false;
            if (CheckAxisSeparation(resultInfo, vertices2, vertices1)) return false;

            //Detected a collision
            FinalizeCollisionInfo(resultInfo, swap, GetCenter(vertices1), GetCenter(vertices2), entity1, entity2, colliderTransform1.IsDynamic, colliderTransform2.IsDynamic);
            CollisionCorrection(resultInfo, swap, colliderTransform1, colliderTransform2);
            GetContactConvexConvex(resultInfo, vertices1, vertices2);

            return true;
      }

      static void inline FinalizeCollisionInfo(CollisionInfo& resultInfo, bool swap, const Vector2& center1, const Vector2& center2, Entity entity1, Entity entity2, bool isDynamic1, bool isDynamic2)
      {
            Vector2 direction = center2 - center1;
            if (direction.Dot(resultInfo.Normal) < 0)
            {
                  resultInfo.Normal = -resultInfo.Normal;
            }

            resultInfo.Normal *= swap ? -1 : 1;
            resultInfo.Entity1 = swap ? entity2 : entity1;
            resultInfo.Entity2 = swap ? entity1 : entity2;
            resultInfo.IsDynamic1 = swap ? isDynamic2 : isDynamic1;
            resultInfo.IsDynamic2 = swap ? isDynamic1 : isDynamic2;
      }


      //SAT functions

      static bool CheckAxisSeparation(CollisionInfo& resultInfo, const std::vector<Vector2>& vertices1, const std::vector<Vector2>& vertices2)
      {
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
                        return true;
                  }

                  Fixed16_16 axisDepth = fpm::min(max2 - min1, max1 - min2);
                  if (axisDepth < resultInfo.Depth)
                  {
                        resultInfo.Depth = axisDepth;
                        resultInfo.Normal = axis;
                  }
            }

            return false;
      }

      static bool CheckCircleAxisSeparation(CollisionInfo& resultInfo, const std::vector<Vector2>& vertices, const Vector2& circlePosition, Fixed16_16 circleRadius, const Vector2& axis)
      {
            Fixed16_16 min1, max1, min2, max2;
            ProjectVertices(vertices, axis, min1, max1);
            ProjectCircle(circlePosition, circleRadius, axis, min2, max2);

            if (min1 >= max2 || min2 >= max1)
            {
                  return true;
            }

            Fixed16_16 axisDepth = min(max2 - min1, max1 - min2);
            if (axisDepth < resultInfo.Depth)
            {
                  resultInfo.Depth = axisDepth;
                  resultInfo.Normal = axis;
            }

            return false;
      }


      //Contact point functions

      static void GetContactCircleCircle(CollisionInfo& collisionInfo, const Vector2& position1, const Vector2& position2, const Fixed16_16& radius1)
      {
            Vector2 direction = (position2 - position1).Normalize();
            collisionInfo.Contact1 = position1 + direction * radius1;
            collisionInfo.ContactCount = 1;
      }

      static void GetContactCircleConvex(CollisionInfo& collisionInfo, const Vector2& circlePosition, const std::vector<Vector2>& vertices)
      {
            long minDistanceSquared = std::numeric_limits<long>::max();
            FindClosestContact(collisionInfo, circlePosition, vertices, minDistanceSquared);
      }

      static void GetContactConvexConvex(CollisionInfo& collisionInfo, const std::vector<Vector2>& vertices1, const std::vector<Vector2>& vertices2)
      {
            long minDistanceSquared = std::numeric_limits<long>::max();

            for (const Vector2& p : vertices1)
            {
                  FindClosestContact(collisionInfo, p, vertices2, minDistanceSquared);
            }
            for (const Vector2& p : vertices2)
            {
                  FindClosestContact(collisionInfo, p, vertices1, minDistanceSquared);
            }
      }

      static void FindClosestContact(CollisionInfo& collisionInfo, const Vector2& point, const std::vector<Vector2>& vertices, long& minDistanceSquared)
      {
            for (int i = 0; i < vertices.size(); ++i)
            {
                  Vector2 v1 = vertices[i];
                  Vector2 v2 = vertices[(i + 1) % vertices.size()];

                  Vector2 contactPoint;
                  long rawDistanceSquared = PointSegmentDistance(point, v1, v2, contactPoint);

                  if (rawDistanceSquared == minDistanceSquared && !Vector2::AlmostEqual(contactPoint, collisionInfo.Contact1))
                  {
                        collisionInfo.ContactCount = 2;
                        collisionInfo.Contact2 = contactPoint;
                  }
                  else if (rawDistanceSquared < minDistanceSquared)
                  {
                        minDistanceSquared = rawDistanceSquared;
                        collisionInfo.ContactCount = 1;
                        collisionInfo.Contact1 = contactPoint;
                  }
            }
      }

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
            long minDistance = std::numeric_limits<long>::max();

            for (auto& vertex : vertices)
            {
                  auto distance = vertex.RawDistanceSquared(center);

                  if (distance < minDistance)
                  {
                        minDistance = distance;
                        result = vertex;
                  }
            }

            return result;
      }

      static long PointSegmentDistance(Vector2 p, Vector2 a, Vector2 b, Vector2& closestPoint)
      {
            Vector2 ab = b - a;
            Vector2 ap = p - a;

            int64_t rawProjection = ap.Dot(ab).raw_value();
            int64_t lengthSquared = ab.RawMagnitudeSquared();

            if (rawProjection <= 0)
            {
                  closestPoint = a;
            }
            else if (rawProjection >= lengthSquared)
            {
                  closestPoint = b;
            }
            else
            {
                  Fixed16_16 d = Fixed16_16::from_raw_value(static_cast<int32_t>((rawProjection << 16) / lengthSquared));
                  closestPoint = a + ab * d;
            }

            return p.RawDistanceSquared(closestPoint);
      }

      static constexpr inline bool AlmostEqual(long x, long y, long epsilon)
      {
            return (x >= y - epsilon) && (x <= y + epsilon);
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
      ComponentCollection<PolygonCollider>* polygonColliderCollection;

      static constexpr Fixed16_16 SmallNumber = Fixed16_16::from_raw_value(16);
};