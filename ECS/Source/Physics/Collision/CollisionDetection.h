#pragma once

#include "CollisionInfo.h"
#include "ContactPair.h"

class CollisionDetection
{
public:
      explicit CollisionDetection(PhysicsComponentManager& componentManager)
      {
            circleColliderCollection = componentManager.GetComponentCollection<CircleCollider>();
            boxColliderCollection = componentManager.GetComponentCollection<BoxCollider>();
            polygonColliderCollection = componentManager.GetComponentCollection<PolygonCollider>();
      }

      bool DetectCollisionAndCorrect(Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2, ContactPair& contactPair) const
      {
            //Skip if none of the objects are dynamic
            if (!colliderTransform1.IsDynamic && !colliderTransform2.IsDynamic) return false;

            //Check for different shape types and do the correct collision detection
            if (colliderTransform1.Shape == Circle)
            {
                  if (colliderTransform2.Shape == Circle)
                  {
                        return CircleCircleCollision(contactPair, entity1, entity2, colliderTransform1, colliderTransform2);
                  }
                  if (colliderTransform2.Shape == Box)
                  {
                        return CircleBoxCollision(contactPair, false, entity1, entity2, colliderTransform1, colliderTransform2);
                  }
                  if (colliderTransform2.Shape == Convex)
                  {
                        return CirclePolygonCollision(contactPair, false, entity1, entity2, colliderTransform1, colliderTransform2);
                  }
            }
            else if (colliderTransform1.Shape == Box)
            {
                  if (colliderTransform2.Shape == Circle)
                  {
                        return CircleBoxCollision(contactPair, true, entity2, entity1, colliderTransform2, colliderTransform1);
                  }
                  if (colliderTransform2.Shape == Box)
                  {
                        return BoxBoxCollision(contactPair, entity1, entity2, colliderTransform1, colliderTransform2);
                  }
                  if (colliderTransform2.Shape == Convex)
                  {
                        return BoxPolygonCollision(contactPair, false, entity1, entity2, colliderTransform1, colliderTransform2);
                  }
            }
            else if (colliderTransform1.Shape == Convex)
            {
                  if (colliderTransform2.Shape == Circle)
                  {
                        return CirclePolygonCollision(contactPair, true, entity2, entity1, colliderTransform2, colliderTransform1);
                  }
                  if (colliderTransform2.Shape == Box)
                  {
                        return BoxPolygonCollision(contactPair, true, entity2, entity1, colliderTransform2, colliderTransform1);
                  }
                  if (colliderTransform2.Shape == Convex)
                  {
                        return PolygonPolygonCollision(contactPair, entity1, entity2, colliderTransform1, colliderTransform2);
                  }
            }

            return false;
      }

      bool CircleCircleCollision(ContactPair& contactPair, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2) const
      {
            assert(circleColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Circle) attached");
            assert(circleColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Circle) attached");

            //Get the components
            CircleCollider& circleCollider1 = circleColliderCollection->GetComponent(entity1);
            CircleCollider& circleCollider2 = circleColliderCollection->GetComponent(entity2);

            //First perform an AABB check, to test if the objects are even able to collide
            if (!colliderTransform1.GetAABB(circleCollider1.GetRadius()).Overlaps(colliderTransform2.GetAABB(circleCollider2.GetRadius())))
            {
                  return false;
            }

            Fixed16_16 distance = colliderTransform1.Position.Distance(colliderTransform2.Position);
            Fixed16_16 totalRadius = circleCollider1.GetRadius() + circleCollider2.GetRadius();

            if (distance >= totalRadius)
            {
                  //No collision
                  return false;
            }

            //Create contact data
            contactPair.ContactCount = 1;
            contactPair.Normal = (colliderTransform2.Position - colliderTransform1.Position).Normalize();
            contactPair.Contacts[0].Position = colliderTransform1.Position + contactPair.Normal * circleCollider1.GetRadius();
            contactPair.Contacts[0].Separation = distance - totalRadius;

            CreateContactData(contactPair, false, colliderTransform1.Position, colliderTransform2.Position, entity1, entity2, colliderTransform1, colliderTransform2);
            return true;
      }

      bool CircleBoxCollision(ContactPair& contactPair, bool swap, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2) const
      {
            assert(circleColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Circle) attached");
            assert(boxColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Box) attached");

            //Get the components
            CircleCollider& circleCollider1 = circleColliderCollection->GetComponent(entity1);
            BoxCollider& boxCollider2 = boxColliderCollection->GetComponent(entity2);

            Vector2Span vertices = colliderTransform2.GetTransformedVertices(boxCollider2.GetTransformedVertices(), boxCollider2.GetVertices());

            return CircleConvexCollision(contactPair, swap, entity1, entity2, colliderTransform1, colliderTransform2, circleCollider1.GetRadius(), vertices);
      }

      bool CirclePolygonCollision(ContactPair& contactPair, bool swap, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2) const
      {
            assert(circleColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Circle) attached");
            assert(polygonColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Polygon) attached");

            //Get the components
            CircleCollider& circleCollider1 = circleColliderCollection->GetComponent(entity1);
            PolygonCollider& polygonCollider2 = polygonColliderCollection->GetComponent(entity2);

            Vector2Span vertices = colliderTransform2.GetTransformedVertices(polygonCollider2.GetTransformedVertices(), polygonCollider2.GetVertices());

            return CircleConvexCollision(contactPair, swap, entity1, entity2, colliderTransform1, colliderTransform2, circleCollider1.GetRadius(), vertices);
      }

      bool BoxBoxCollision(ContactPair& contactPair, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2) const
      {
            assert(boxColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Box) attached");
            assert(boxColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Box) attached");

            //Get the components
            BoxCollider& boxCollider1 = boxColliderCollection->GetComponent(entity1);
            BoxCollider& boxCollider2 = boxColliderCollection->GetComponent(entity2);

            Vector2Span vertices1 = colliderTransform1.GetTransformedVertices(boxCollider1.GetTransformedVertices(), boxCollider1.GetVertices());
            Vector2Span vertices2 = colliderTransform2.GetTransformedVertices(boxCollider2.GetTransformedVertices(), boxCollider2.GetVertices());

            return ConvexConvexCollision(contactPair, false, entity1, entity2, colliderTransform1, colliderTransform2, vertices1, vertices2);
      }

      bool BoxPolygonCollision(ContactPair& contactPair, bool swap, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2) const
      {
            assert(boxColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Box) attached");
            assert(polygonColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Polygon) attached");

            //Get the components
            BoxCollider& boxCollider1 = boxColliderCollection->GetComponent(entity1);
            PolygonCollider& polygonCollider2 = polygonColliderCollection->GetComponent(entity2);

            Vector2Span vertices1 = colliderTransform1.GetTransformedVertices(boxCollider1.GetTransformedVertices(), boxCollider1.GetVertices());
            Vector2Span vertices2 = colliderTransform2.GetTransformedVertices(polygonCollider2.GetTransformedVertices(), polygonCollider2.GetVertices());

            return ConvexConvexCollision(contactPair, swap, entity1, entity2, colliderTransform1, colliderTransform2, vertices1, vertices2);
      }

      bool PolygonPolygonCollision(ContactPair& contactPair, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2) const
      {
            assert(polygonColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Polygon) attached");
            assert(polygonColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Polygon) attached");

            //Get the components
            PolygonCollider& polygonCollider1 = polygonColliderCollection->GetComponent(entity1);
            PolygonCollider& polygonCollider2 = polygonColliderCollection->GetComponent(entity2);

            Vector2Span vertices1 = colliderTransform1.GetTransformedVertices(polygonCollider1.GetTransformedVertices(), polygonCollider1.GetVertices());
            Vector2Span vertices2 = colliderTransform2.GetTransformedVertices(polygonCollider2.GetTransformedVertices(), polygonCollider2.GetVertices());

            return ConvexConvexCollision(contactPair, false, entity1, entity2, colliderTransform1, colliderTransform2, vertices1, vertices2);
      }

      static bool CircleConvexCollision(ContactPair& contactPair, bool swap, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2, const Fixed16_16& circleRadius, Vector2Span vertices)
      {
            //First perform an AABB check, to test if the objects are even able to collide
            if (!colliderTransform1.GetAABB(circleRadius).Overlaps(colliderTransform2.GetAABBFromTransformed(vertices)))
            {
                  //No collision possible
                  return false;
            }

            contactPair.Contacts[0].Separation = std::numeric_limits<Fixed16_16>::max();

            for(int i = 0; i < vertices.size; ++i)
            {
                  Vector2 v1 = vertices[i];
                  Vector2 v2 = vertices[(i + 1) % vertices.size];

                  Vector2 edge = v2 - v1;
                  Vector2 axis = edge.Perpendicular().Normalize();

                  if (CheckCircleAxisSeparation(contactPair, vertices, colliderTransform1.Position, circleRadius, axis)) return false;
            }

            Vector2 closestVertexToCircle = GetClosestPointToCircle(colliderTransform1.Position, vertices);
            Vector2 axis = (closestVertexToCircle - colliderTransform1.Position).Normalize();

            if (CheckCircleAxisSeparation(contactPair, vertices, colliderTransform1.Position, circleRadius, axis)) return false;

            contactPair.Contacts[0].Separation = -contactPair.Contacts[0].Separation;
            contactPair.Contacts[1].Separation = contactPair.Contacts[0].Separation;

            //Detected collision
            GetContactCircleConvex(contactPair, colliderTransform1.Position, vertices);

            //Create contact data
            CreateContactData(contactPair, swap, colliderTransform1.Position, GetCenter(vertices), entity1, entity2, colliderTransform1, colliderTransform2);
            return true;
      }

      static bool ConvexConvexCollision(ContactPair& contactPair, bool swap, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2, Vector2Span vertices1, Vector2Span vertices2 )
      {
            //First perform an AABB check, to test if the objects are even able to collide
            if (!colliderTransform1.GetAABBFromTransformed(vertices1).Overlaps(colliderTransform2.GetAABBFromTransformed(vertices2))) return false;

            //contactPair.Penetration = -std::numeric_limits<Fixed16_16>::max();

            //if (CheckAxisSeparation(contactPair, vertices1, vertices2, true)) return false;
            //if (CheckAxisSeparation(contactPair, vertices2, vertices1, false)) return false;

            ContactPair contactPair2;
            bool b = ConvexConvexCollision2(contactPair2, swap, entity1, entity2, colliderTransform1, colliderTransform2, vertices1, vertices2);

            if (!b) return false;

            contactPair.Penetration = contactPair2.Penetration;
            contactPair.Normal = contactPair2.Normal;
            contactPair.ContactCount = contactPair2.ContactCount;
            contactPair.Contacts = contactPair2.Contacts;
            contactPair.Contacts[0].Separation = -contactPair.Penetration;
            contactPair.Contacts[1].Separation = -contactPair.Penetration;

            //Detected a collision
            //GetContactConvexConvex2(contactPair, vertices1, vertices2);

            //if (contactPair2.ContactCount == 0) return false;

            //Create contact data
            CreateContactData(contactPair, swap, GetCenter(vertices1), GetCenter(vertices2), entity1, entity2, colliderTransform1, colliderTransform2);
            return true;
      }

      static void inline CreateContactData(ContactPair& contactPair, bool swap, const Vector2& center1, const Vector2& center2, Entity entity1, Entity entity2, const ColliderTransform& colliderTransform1, const ColliderTransform& colliderTransform2)
      {
            Vector2 direction = center2 - center1;
            if (direction.Dot(contactPair.Normal) < 0)
            {
                 contactPair.Normal = -contactPair.Normal;
            }

            if (swap)
            {
                  contactPair.Entity1 = entity2;
                  contactPair.Entity2 = entity1;

                  for (int i = 0; i < contactPair.ContactCount; ++i)
                  {
                        Contact& contact = contactPair.Contacts[i];
                        contact.R1 = contact.Position - colliderTransform2.Position;
                        contact.R2 = contact.Position - colliderTransform1.Position;
                        contact.Pn = Fixed16_16(0);
                        contact.Pt = Fixed16_16(0);
                        contact.Pnb = Fixed16_16(0);
                        contact.Feature.value = 0;
                  }

                  contactPair.Normal = -contactPair.Normal;
            }
            else
            {
                  contactPair.Entity1 = entity1;
                  contactPair.Entity2 = entity2;

                  for (int i = 0; i < contactPair.ContactCount; ++i)
                  {
                        Contact& contact = contactPair.Contacts[i];
                        contact.R1 = contact.Position - colliderTransform1.Position;
                        contact.R2 = contact.Position - colliderTransform2.Position;
                        contact.Pn = Fixed16_16(0);
                        contact.Pt = Fixed16_16(0);
                        contact.Pnb = Fixed16_16(0);
                        contact.Feature.value = 0;
                  }
            }

            contactPair.Friction = Fixed16_16(1) / Fixed16_16(5); //TODO
      }


      //SAT functions

      static bool CheckAxisSeparation(ContactPair& contactPair, Vector2Span vertices1, Vector2Span vertices2, bool refIsPoly1)
      {
            for(int i = 0; i < vertices1.size; ++i)
            {
                  Vector2 v1 = vertices1[i];
                  Vector2 v2 = vertices1[(i + 1) % vertices1.size];

                  Vector2 edge = v2 - v1;
                  Vector2 axis = (-edge).Perpendicular().Normalize();

                  Fixed16_16 min1, max1, min2, max2;
                  ProjectVertices(vertices1, axis, min1, max1);
                  ProjectVertices(vertices2, axis, min2, max2);

                  if (min1 >= max2 || min2 >= max1)
                  {
                        //Separation detected - No collision
                        return true;
                  }

                  Fixed16_16 axisDepth = fpm::min(max2 - min1, max1 - min2);
                  if (axisDepth > contactPair.Penetration)
                  {
                        contactPair.Penetration = axisDepth;
                        contactPair.Normal = axis;
                        contactPair.RefIsPoly1 = refIsPoly1;
                        contactPair.RefEdgeIndex = i;
                  }
            }

            return false;
      }

      static bool CheckCircleAxisSeparation(ContactPair& contactPair, Vector2Span vertices, const Vector2& circlePosition, Fixed16_16 circleRadius, const Vector2& axis)
      {
            Fixed16_16 min1, max1, min2, max2;
            ProjectVertices(vertices, axis, min1, max1);
            ProjectCircle(circlePosition, circleRadius, axis, min2, max2);

            if (min1 >= max2 || min2 >= max1)  return true;

            Fixed16_16 axisDepth = min(max2 - min1, max1 - min2);
            if (axisDepth < contactPair.Contacts[0].Separation)
            {
                  contactPair.Contacts[0].Separation = axisDepth;
                  contactPair.Normal = axis;
            }

            return false;
      }


      //Contact point functions

      static inline void GetContactCircleConvex(ContactPair& contactPair, Vector2 circlePosition, Vector2Span vertices)
      {
            long minDistanceSquared = std::numeric_limits<long>::max();
            FindClosestContact(contactPair, circlePosition, vertices, minDistanceSquared);
      }

      static inline void GetContactConvexConvex2(ContactPair& contactPair, Vector2Span vertices1, Vector2Span vertices2)
      {
            long minDistanceSquared = std::numeric_limits<long>::max();

            for (const Vector2& vertex : vertices1)
            {
                  FindClosestContact(contactPair, vertex, vertices2, minDistanceSquared);
            }
            for (const Vector2& vertex : vertices2)
            {
                  FindClosestContact(contactPair, vertex, vertices1, minDistanceSquared);
            }
      }

      static void FindClosestContact(ContactPair& contactPair, Vector2 point, Vector2Span vertices, long& minDistanceSquared)
      {
            for (int i = 0; i < vertices.size; ++i)
            {
                  Vector2 v1 = vertices[i];
                  Vector2 v2 = vertices[(i + 1) % vertices.size];

                  Vector2 contactPoint;
                  long rawDistanceSquared = PointSegmentDistance(point, v1, v2, contactPoint);

                  if (rawDistanceSquared == minDistanceSquared && !Vector2::AlmostEqual(contactPoint, contactPair.Contacts[0].Position)) //todo pot issue
                  {
                        contactPair.ContactCount = 2;
                        contactPair.Contacts[1].Position = contactPoint;
                  }
                  else if (rawDistanceSquared < minDistanceSquared)
                  {
                        minDistanceSquared = rawDistanceSquared;
                        contactPair.ContactCount = 1;
                        contactPair.Contacts[0].Position = contactPoint;
                  }
            }
      }

      static void ProjectVertices(Vector2Span vertices, const Vector2& axis, Fixed16_16& min, Fixed16_16& max)
      {
            min = std::numeric_limits<Fixed16_16>::max();
            max = -std::numeric_limits<Fixed16_16>::max();

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

      static Vector2 GetCenter(Vector2Span vertices)
      {
            Vector2 sum(0, 0);

            for (auto& vertex : vertices)
            {
                  sum += vertex;
            }

            return sum / vertices.size;
      }

      static Vector2 GetClosestPointToCircle(Vector2 center, Vector2Span vertices)
      {
            Vector2 result(0, 0);
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
      ComponentCollection<CircleCollider>* circleColliderCollection;
      ComponentCollection<BoxCollider>* boxColliderCollection;
      ComponentCollection<PolygonCollider>* polygonColliderCollection;

public:
      /////////////////////////////////////
      ///Test 2

      static bool ConvexConvexCollision2(ContactPair& contactPair, bool swap, Entity entity1, Entity entity2, ColliderTransform& colliderTransform1, ColliderTransform& colliderTransform2, Vector2Span vertices1, Vector2Span vertices2)
      {
            //First perform an AABB check, to test if the objects are even able to collide
            if (!colliderTransform1.GetAABBFromTransformed(vertices1).Overlaps(colliderTransform2.GetAABBFromTransformed(vertices2))) return false;

            if (!CollisionHelper(vertices1, vertices2,contactPair)) return false;

            Manifold manifold;
            if (!BuildManifold(vertices1, vertices2, contactPair, manifold)) return false;

            contactPair.ContactCount = manifold.count;
            contactPair.Contacts[0].Position = manifold.points[0].point;
            contactPair.Contacts[1].Position = manifold.points[1].point;
            contactPair.Normal = manifold.normal;
            contactPair.Penetration = manifold.penetration;

            return true;
      }

      struct PolygonProjection
      {
            Fixed16_16 lo;
            Fixed16_16 hi;
      };

      struct CollisionResult
      {
            Fixed16_16 Penetration;
            Vector2 Normal;
      };

      static PolygonProjection ProjectPolygonAlongAxis(Vector2Span vertices, const Vector2 normal)
      {
            PolygonProjection proj;
            proj.lo = std::numeric_limits<Fixed16_16>::max();
            proj.hi = -std::numeric_limits<Fixed16_16>::max();
            for (uint8_t i = 0; i < vertices.size; i++)
            {
                  Fixed16_16 d = normal.Dot(vertices[i]);
                  proj.lo = std::min(d, proj.lo);
                  proj.hi = std::max(d, proj.hi);
            }
            return proj;
      }

      // Separating Axis Theorem for convex polygons
      static bool CollisionHelper(Vector2Span vertices1, Vector2Span vertices2, ContactPair& contactPair)
      {
            Fixed16_16 minPenetration = std::numeric_limits<Fixed16_16>::max();
            Vector2 bestNormal = Vector2(0, 0);

            auto checkEdges = [&](Vector2Span poly)
            {
                  for (uint8_t i = 0; i < poly.size; i++)
                  {
                        Vector2 a = poly[i];
                        Vector2 b = poly[(i + 1) % poly.size];
                        Vector2 edge = b - a;

                        assert(edge != Vector2(0, 0) && "Vertices on polygon should not overlap");

                        Vector2 normal = edge.PerpendicularInverse().Normalize();

                        PolygonProjection projA = ProjectPolygonAlongAxis(vertices1, normal);
                        PolygonProjection projB = ProjectPolygonAlongAxis(vertices2, normal);

                        if (projA.hi < projB.lo || projB.hi < projA.lo) return false; //Separating axis found

                        Fixed16_16 overlap = fpm::min(projA.hi - projB.lo, projB.hi - projA.lo);
                        if (overlap < minPenetration)
                        {
                              minPenetration = overlap;
                              bestNormal = normal;
                        }
                  }

                  return true;
            };

            if (!checkEdges(vertices1)) return false;
            if (!checkEdges(vertices2)) return false;

            // Collision confirmed
            contactPair.Penetration = minPenetration;

            // Ensure normal points from A to B
            if ((GetCenter(vertices2) - GetCenter(vertices1)).Dot(bestNormal) < 0)
                  bestNormal = -bestNormal;

            contactPair.Normal = bestNormal;

            return true;
      }

      struct ManifoldPoint
      {
            Vector2 point = Vector2(0, 0);
            Fixed16_16 separation = Fixed16_16(0);
      };

      struct Manifold
      {
            uint8_t count = 0;
            Vector2 normal = Vector2(0, 0);
            Fixed16_16 penetration = Fixed16_16(0);
            ManifoldPoint points[2];
      };

      static inline Vector2 EdgeNormal(Vector2Span vertices, uint8_t i)
      {
            uint8_t j = (i + 1) % vertices.size;
            Vector2 e = vertices[j] - vertices[i];
            return e.PerpendicularInverse().Normalize();
      }

      static int FindReferenceEdge(Vector2Span vertices, Vector2 normal)
      {
            Fixed16_16 maxDot = -std::numeric_limits<Fixed16_16>::max();
            int idx = 0;

            for (int i = 0; i < vertices.size; ++i)
            {
                  Vector2 n = EdgeNormal(vertices, i);
                  Fixed16_16 d = n.Dot(normal);
                  if (d > maxDot)
                  {
                        maxDot = d;
                        idx = i;
                  }
            }

            return idx;
      }

      static int FindIncidentEdge(Vector2Span vertices, Vector2 referenceNormal)
      {
            Fixed16_16 minDot = std::numeric_limits<Fixed16_16>::max();
            int idx = 0;

            for (int i = 0; i < vertices.size; ++i)
            {
                  Fixed16_16 d = referenceNormal.Dot(EdgeNormal(vertices, i));
                  if (d < minDot)
                  {
                        minDot = d;
                        idx = i;
                  }
            }

            return idx;
      }

      struct ClipResult
      {
            int n = 0;
            Vector2 pts[2];
      };

      static ClipResult ClipSegmentToHalfspace(const Vector2 a, const Vector2 b, const Vector2 n, const Fixed16_16 o)
      {
            ClipResult out;

            Fixed16_16 da = n.Dot(a) - o;
            Fixed16_16 db = n.Dot(b) - o;

            //Keep points on or inside (<= 0)
            if (da <= Fixed16_16(0)) { out.pts[out.n++] = a; }
            if (db <= Fixed16_16(0)) { out.pts[out.n++] = b; }

            //Check if the points are on different sides
            if ((da < Fixed16_16(0) && db > Fixed16_16(0)) || (da > Fixed16_16(0) && db < Fixed16_16(0)))
            {
                  Fixed16_16 t = da / (da - db);
                  Vector2 p = a + (b - a) * t;
                  out.pts[out.n++] = p;
            }

            return out;
      }

      static bool BuildManifold(Vector2Span vertices1, Vector2Span vertices2, ContactPair& contactPair, Manifold& manifold)
      {
            //Decide reference vs incident polygon by which face is most aligned with SAT normal
            int refEdgeA = FindReferenceEdge(vertices1, contactPair.Normal);
            int refEdgeB = FindReferenceEdge(vertices2, contactPair.Normal);

            Fixed16_16 dotA = EdgeNormal(vertices1, refEdgeA).Dot(contactPair.Normal);
            Fixed16_16 dotB = EdgeNormal(vertices2, refEdgeB).Dot(contactPair.Normal);

            if (dotA < dotB)
            {
                  contactPair.Normal = -contactPair.Normal;
                  swap(vertices1, vertices2);
                  refEdgeA = FindReferenceEdge(vertices1, contactPair.Normal);
            }

            Vector2Span Ref = vertices1;
            Vector2Span Inc = vertices2;
            int refEdge = refEdgeA;

            //Reference edge endpoints and basis (tangent + normal)
            int refSucc = (refEdge + 1) % Ref.size;
            Vector2 refA = Ref[refEdge];
            Vector2 refB = Ref[refSucc];
            Vector2 refTangent = (refB - refA).Normalize();
            Vector2 refNormal = refTangent.PerpendicularInverse().Normalize();

            if (refNormal.Dot(contactPair.Normal) < Fixed16_16(0)) refNormal = -refNormal;

            //Find incident edge from the other polygon
            int incEdge = FindIncidentEdge(Inc, refNormal);
            int incSucc = (incEdge + 1) % Inc.size;
            Vector2 incA = Inc[incEdge];
            Vector2 incB = Inc[incSucc];

            //Clip the incident edge against the two side planes of the reference face
            const Vector2 nL = -refTangent;
            const Fixed16_16 oL = -refTangent.Dot(refA);
            const Vector2 nR = refTangent;
            const Fixed16_16 oR =  refTangent.Dot(refB);

            ClipResult c1 = ClipSegmentToHalfspace(incA, incB, nL, oL);
            if (c1.n < 2) return false; //Only possible with imprecision

            ClipResult c2 = ClipSegmentToHalfspace(c1.pts[0], c1.pts[1], nR, oR);
            if (c2.n < 2) return false; //Only possible with imprecision

            //Keep points that are behind the reference face plane
            Fixed16_16 faceOffset = refNormal.Dot(refA);
            Fixed16_16 kEps = Fixed16_16(0); //Add bias here possible todo

            manifold.count = 0;
            manifold.normal = contactPair.Normal;
            manifold.penetration = contactPair.Penetration;

            for (int i = 0; i < c2.n && manifold.count < 2; ++i)
            {
                  Fixed16_16 sep = refNormal.Dot(c2.pts[i]) - faceOffset;
                  if (sep <= kEps)
                  {
                        manifold.points[manifold.count].point = c2.pts[i];
                        manifold.points[manifold.count].separation = sep;
                        manifold.count++;
                  }
            }

            return true;
      }
};

//todo: validate shapes with ccw