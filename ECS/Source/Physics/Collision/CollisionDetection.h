#pragma once

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

      bool DetectCollision(Entity entity1, Entity entity2, Transform& transform1, Transform& transform2, TransformMeta& transformMeta1, TransformMeta& transformMeta2, ContactPair& contactPair) const
      {
            //Skip if none of the objects are dynamic
            if (!transformMeta1.IsDynamic && !transformMeta2.IsDynamic) return false;

            //Check for different shape types and do the correct collision detection
            if (transformMeta1.Shape == Circle)
            {
                  if (transformMeta2.Shape == Circle)
                  {
                        return CircleCircleCollision(contactPair, entity1, entity2, transform1, transform2, transformMeta1, transformMeta2);
                  }
                  if (transformMeta2.Shape == Box)
                  {
                        return CircleBoxCollision(contactPair, false, entity1, entity2, transform1, transform2, transformMeta1, transformMeta2);
                  }
                  if (transformMeta2.Shape == Convex)
                  {
                        return CirclePolygonCollision(contactPair, false, entity1, entity2, transform1, transform2, transformMeta1, transformMeta2);
                  }
            }
            else if (transformMeta1.Shape == Box)
            {
                  if (transformMeta2.Shape == Circle)
                  {
                        return CircleBoxCollision(contactPair, true, entity2, entity1, transform2, transform1, transformMeta2, transformMeta1);
                  }
                  if (transformMeta2.Shape == Box)
                  {
                        return BoxBoxCollision(contactPair, entity1, entity2, transform1, transform2, transformMeta1, transformMeta2);
                  }
                  if (transformMeta2.Shape == Convex)
                  {
                        return BoxPolygonCollision(contactPair, false, entity1, entity2, transform1, transform2, transformMeta1, transformMeta2);
                  }
            }
            else if (transformMeta1.Shape == Convex)
            {
                  if (transformMeta2.Shape == Circle)
                  {
                        return CirclePolygonCollision(contactPair, true, entity2, entity1, transform2, transform1, transformMeta2, transformMeta1);
                  }
                  if (transformMeta2.Shape == Box)
                  {
                        return BoxPolygonCollision(contactPair, true, entity2, entity1, transform2, transform1, transformMeta2, transformMeta1);
                  }
                  if (transformMeta2.Shape == Convex)
                  {
                        return PolygonPolygonCollision(contactPair, entity1, entity2, transform1, transform2, transformMeta1, transformMeta2);
                  }
            }

            return false;
      }

      bool CircleCircleCollision(ContactPair& contactPair, Entity entity1, Entity entity2, Transform& transform1, Transform& transform2, TransformMeta& transformMeta1, TransformMeta& transformMeta2) const
      {
            assert(circleColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Circle) attached");
            assert(circleColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Circle) attached");

            //Get the components
            CircleCollider& circleCollider1 = circleColliderCollection->GetComponent(entity1);
            CircleCollider& circleCollider2 = circleColliderCollection->GetComponent(entity2);

            //Perform AABB check, to test if entities are able to collide
            if (!circleCollider1.GetAABB(transform1, transformMeta1).Overlaps(circleCollider2.GetAABB(transform2, transformMeta2))) return false;

            Fixed16_16 distance = transform1.Base.Position.Distance(transform2.Base.Position);
            Fixed16_16 totalRadius = circleCollider1.GetRadius() + circleCollider2.GetRadius();

            //Check if circles overlap
            if (distance >= totalRadius) return false;

            //Create contact data
            contactPair.ContactCount = 1;
            contactPair.Normal = (transform2.Base.Position - transform1.Base.Position).Normalize();
            contactPair.Contacts[0].Position = transform1.Base.Position + contactPair.Normal * circleCollider1.GetRadius();
            contactPair.Contacts[0].Separation = distance - totalRadius;

            CreateContactData(contactPair, false, transform1.Base.Position, transform2.Base.Position, entity1, entity2, transform1, transform2);
            return true;
      }

      bool CircleBoxCollision(ContactPair& contactPair, bool swap, Entity entity1, Entity entity2, Transform& transform1, Transform& transform2, TransformMeta& transformMeta1, TransformMeta& transformMeta2) const
      {
            assert(circleColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Circle) attached");
            assert(boxColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Box) attached");

            //Get the components
            CircleCollider& circleCollider1 = circleColliderCollection->GetComponent(entity1);
            BoxCollider& boxCollider2 = boxColliderCollection->GetComponent(entity2);

            //Perform AABB check, to test if entities are able to collide
            if (!circleCollider1.GetAABB(transform1, transformMeta1).Overlaps(boxCollider2.GetAABB(transform2, transformMeta2))) return false;

            Vector2Span vertices = boxCollider2.GetTransformedVertices(transform2);
            return CircleConvexCollision(contactPair, swap, entity1, entity2, transform1, transform2, circleCollider1.GetRadius(), vertices);
      }

      bool CirclePolygonCollision(ContactPair& contactPair, bool swap, Entity entity1, Entity entity2, Transform& transform1, Transform& transform2, TransformMeta& transformMeta1, TransformMeta& transformMeta2) const
      {
            assert(circleColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Circle) attached");
            assert(polygonColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Polygon) attached");

            //Get the components
            CircleCollider& circleCollider1 = circleColliderCollection->GetComponent(entity1);
            PolygonCollider& polygonCollider2 = polygonColliderCollection->GetComponent(entity2);

            //Perform AABB check, to test if entities are able to collide
            if (!circleCollider1.GetAABB(transform1, transformMeta1).Overlaps(polygonCollider2.GetAABB(transform2, transformMeta2))) return false;

            Vector2Span vertices = polygonCollider2.GetTransformedVertices(transform2);
            return CircleConvexCollision(contactPair, swap, entity1, entity2, transform1, transform2, circleCollider1.GetRadius(), vertices);
      }

      bool BoxBoxCollision(ContactPair& contactPair, Entity entity1, Entity entity2, Transform& transform1, Transform& transform2, TransformMeta& transformMeta1, TransformMeta& transformMeta2) const
      {
            assert(boxColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Box) attached");
            assert(boxColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Box) attached");

            //Get the components
            BoxCollider& boxCollider1 = boxColliderCollection->GetComponent(entity1);
            BoxCollider& boxCollider2 = boxColliderCollection->GetComponent(entity2);

            //Perform AABB check, to test if entities are able to collide
            if (!boxCollider1.GetAABB(transform1, transformMeta1).Overlaps(boxCollider2.GetAABB(transform2, transformMeta2))) return false;

            Vector2Span vertices1 = boxCollider1.GetTransformedVertices(transform1);
            Vector2Span vertices2 = boxCollider2.GetTransformedVertices(transform2);
            return ConvexConvexCollision(contactPair, false, entity1, entity2, transform1, transform2, vertices1, vertices2);
      }

      bool BoxPolygonCollision(ContactPair& contactPair, bool swap, Entity entity1, Entity entity2, Transform& transform1, Transform& transform2, TransformMeta& transformMeta1, TransformMeta& transformMeta2) const
      {
            assert(boxColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Box) attached");
            assert(polygonColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Polygon) attached");

            //Get the components
            BoxCollider& boxCollider1 = boxColliderCollection->GetComponent(entity1);
            PolygonCollider& polygonCollider2 = polygonColliderCollection->GetComponent(entity2);

            //Perform AABB check, to test if entities are able to collide
            if (!boxCollider1.GetAABB(transform1, transformMeta1).Overlaps(polygonCollider2.GetAABB(transform2, transformMeta2))) return false;

            Vector2Span vertices1 = boxCollider1.GetTransformedVertices(transform1);
            Vector2Span vertices2 = polygonCollider2.GetTransformedVertices(transform2);

            return ConvexConvexCollision(contactPair, swap, entity1, entity2, transform1, transform2, vertices1, vertices2);
      }

      bool PolygonPolygonCollision(ContactPair& contactPair, Entity entity1, Entity entity2, Transform& transform1, Transform& transform2, TransformMeta& transformMeta1, TransformMeta& transformMeta2) const
      {
            assert(polygonColliderCollection->HasComponent(entity1) && "Collider type of rigidBody does not have the correct collider (Polygon) attached");
            assert(polygonColliderCollection->HasComponent(entity2) && "Collider type of rigidBody does not have the correct collider (Polygon) attached");

            //Get the components
            PolygonCollider& polygonCollider1 = polygonColliderCollection->GetComponent(entity1);
            PolygonCollider& polygonCollider2 = polygonColliderCollection->GetComponent(entity2);

            //Perform AABB check, to test if entities are able to collide
            if (!polygonCollider1.GetAABB(transform1, transformMeta1).Overlaps(polygonCollider2.GetAABB(transform2, transformMeta2))) return false;

            Vector2Span vertices1 = polygonCollider1.GetTransformedVertices(transform1);
            Vector2Span vertices2 = polygonCollider2.GetTransformedVertices(transform2);

            return ConvexConvexCollision(contactPair, false, entity1, entity2, transform1, transform2, vertices1, vertices2);
      }

private:
      static bool CircleConvexCollision(ContactPair& contactPair, bool swap, Entity entity1, Entity entity2, const Transform& transform1, const Transform& transform2, const Fixed16_16& circleRadius, Vector2Span vertices)
      {
            contactPair.Contacts[0].Separation = std::numeric_limits<Fixed16_16>::max();

            for(int i = 0; i < vertices.size; ++i)
            {
                  Vector2 v1 = vertices[i];
                  Vector2 v2 = vertices[(i + 1) % vertices.size];

                  Vector2 edge = v2 - v1;
                  Vector2 axis = edge.Perpendicular().Normalize();

                  if (CheckCircleAxisSeparation(contactPair, vertices, transform1.Base.Position, circleRadius, axis)) return false;
            }

            Vector2 closestVertexToCircle = GetClosestPointToCircle(transform1.Base.Position, vertices);
            Vector2 axis = (closestVertexToCircle - transform1.Base.Position).Normalize();

            if (CheckCircleAxisSeparation(contactPair, vertices, transform1.Base.Position, circleRadius, axis)) return false;

            contactPair.Contacts[0].Separation = -contactPair.Contacts[0].Separation;
            contactPair.Contacts[1].Separation = contactPair.Contacts[0].Separation;

            //Detected collision
            GetContactCircleConvex(contactPair, transform1.Base.Position, vertices);

            //Create contact data
            CreateContactData(contactPair, swap, transform1.Base.Position, GetCenter(vertices), entity1, entity2, transform1, transform2);
            return true;
      }

      static bool ConvexConvexCollision(ContactPair& contactPair, bool swap, Entity entity1, Entity entity2, const Transform& transform1, const Transform& transform2, Vector2Span vertices1, Vector2Span vertices2 )
      {
            assert(vertices1.size > 0 && vertices2.size > 0 && "Polygon cannot have zero vertices");

            Vector2 center1 = GetCenter(vertices1);
            Vector2 center2 = GetCenter(vertices2);

            OverlapData overlap1;
            OverlapData overlap2;

            if (!ProjectEdges(overlap1, vertices1, vertices2, center1, center2)) return false;
            if (!ProjectEdges(overlap2, vertices2, vertices1, center2, center1)) return false;

            Vector2Span Reference, Incident;
            OverlapData resultOverlap;

            if (BiasGreaterThan(overlap2.Penetration,overlap1.Penetration))
            {
                  Reference = vertices1;
                  Incident = vertices2;
                  resultOverlap = overlap1;
                  resultOverlap.Flipped = false; //Use flipped as indication for the feature pair
            }
            else
            {
                  Reference = vertices2;
                  Incident = vertices1;
                  resultOverlap = overlap2;
                  resultOverlap.Flipped = true; //Use flipped as indication for the feature pair
            }

            if (!BuildManifold(contactPair, Reference, Incident, resultOverlap)) return false;

            CreateContactData(contactPair, swap, center1, center2, entity1, entity2, transform1, transform2);
            return true;
      }

      static void inline CreateContactData(ContactPair& contactPair, bool swap, const Vector2& center1, const Vector2& center2, Entity entity1, Entity entity2, const Transform& transform1, const Transform& transform2)
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
                  contactPair.EntityKey = EntityPair::Make(entity2, entity1);

                  for (int i = 0; i < contactPair.ContactCount; ++i)
                  {
                        Contact& contact = contactPair.Contacts[i];
                        contact.R1 = contact.Position - transform2.Base.Position;
                        contact.R2 = contact.Position - transform1.Base.Position;
                  }

                  contactPair.Normal = -contactPair.Normal;
            }
            else
            {
                  contactPair.Entity1 = entity1;
                  contactPair.Entity2 = entity2;
                  contactPair.EntityKey = EntityPair::Make(entity1, entity2);

                  for (int i = 0; i < contactPair.ContactCount; ++i)
                  {
                        Contact& contact = contactPair.Contacts[i];
                        contact.R1 = contact.Position - transform1.Base.Position;
                        contact.R2 = contact.Position - transform2.Base.Position;
                  }
            }

            contactPair.Friction = Fixed16_16(1) / Fixed16_16(3); //TODO
      }

      //SAT functions
      struct OverlapData
      {
            Fixed16_16 Penetration;
            Vector2 Normal;
            uint8_t Edge;
            bool Flipped;
      };

      static bool ProjectEdges(OverlapData& overlapData, Vector2Span vertices1, Vector2Span vertices2, Vector2 center1, Vector2 center2)
      {
            overlapData.Penetration = std::numeric_limits<Fixed16_16>::max();
            overlapData.Flipped = false;

            Vector2 centerDirection = center2 - center1;

            for (uint8_t i = 0; i < vertices1.size; i++)
            {
                  Vector2 edge = vertices1[(i + 1) % vertices1.size] - vertices1[i];

                  assert(edge != Vector2(0, 0) && "Vertices on polygon should not overlap");

                  Vector2 normal = edge.PerpendicularInverse().Normalize();
                  bool flipped = false;

                  if (centerDirection.Dot(normal) < Fixed16_16(0))
                  {
                        flipped = true;
                        normal = -normal;
                  }

                  Fixed16_16 minA, maxA, minB, maxB;
                  ProjectVertices(vertices1, normal, minA, maxA);
                  ProjectVertices(vertices2, normal, minB, maxB);

                  if (maxA < minB || maxB < minA) return false;

                  Fixed16_16 penetration = fpm::min(maxA - minB, maxB - minA);

                  if (EvaluatePenetrationBias(overlapData, flipped, penetration))
                  {
                        overlapData.Penetration = penetration;
                        overlapData.Normal = normal;
                        overlapData.Edge = i;
                        overlapData.Flipped = flipped;
                  }
            }

            return true;
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

      static inline void ProjectVertices(Vector2Span vertices, const Vector2 normal, Fixed16_16& min, Fixed16_16& max)
      {
            min = std::numeric_limits<Fixed16_16>::max();
            max = -std::numeric_limits<Fixed16_16>::max();

            for (Vector2 vertex : vertices)
            {
                  Fixed16_16 dot = normal.Dot(vertex);
                  min = std::min(dot, min);
                  max = std::max(dot, max);
            }
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

      static bool BuildManifold(ContactPair& contactPair, Vector2Span ReferenceVertices, Vector2Span IncidentVertices, const OverlapData& overlapData)
      {
            contactPair.Normal = overlapData.Normal;

            Vector2 reference1 = ReferenceVertices[overlapData.Edge];
            Vector2 reference2 = ReferenceVertices[(overlapData.Edge + 1) % ReferenceVertices.size];

            Vector2 refTangent = (reference2 - reference1).Normalize();
            Vector2 refNormal = refTangent.PerpendicularInverse().Normalize();

            if (refNormal.Dot(overlapData.Normal) < Fixed16_16(0))
                  refNormal = -refNormal;

            //Find incident edge from the other polygon
            uint8_t incident1 = FindIncidentEdge(IncidentVertices, refNormal);
            uint8_t incident2 = (incident1 + 1) % IncidentVertices.size;

            //Clip the incident edge against the two side planes of the reference face
            const Vector2 nL = -refTangent;
            const Fixed16_16 oL = -refTangent.Dot(reference1);
            const Vector2 nR = refTangent;
            const Fixed16_16 oR = refTangent.Dot(reference2);

            std::array<Vector2, 2> clipPoints1;
            uint8_t clipCount1 = ClipSegmentToHalfSpace(std::array { IncidentVertices[incident1], IncidentVertices[incident2] }, nL, oL, clipPoints1);
            if (clipCount1 < 2) return false; //Only possible with imprecision

            std::array<Vector2, 2> clipPoints2;
            uint8_t clipCount2 = ClipSegmentToHalfSpace(clipPoints1, nR, oR, clipPoints2);
            if (clipCount2 < 2) return false; //Only possible with imprecision

            //Keep points that are behind the reference face plane
            Fixed16_16 faceOffset = refNormal.Dot(reference1);
            constexpr Fixed16_16 maxSeparation = Fixed16_16(0); //Add bias here possible todo

            contactPair.ContactCount = 0;

            for (int i = 0; i < clipCount2 && contactPair.ContactCount < 2; ++i)
            {
                  Fixed16_16 separation = refNormal.Dot(clipPoints2[i]) - faceOffset;
                  if (separation <= maxSeparation)
                  {
                        auto& contact = contactPair.Contacts[contactPair.ContactCount];
                        contact.Position = clipPoints2[i];
                        contact.Separation = separation;
                        contact.LastImpulse.Feature.edges.ReferenceEdge = overlapData.Edge;
                        contact.LastImpulse.Feature.edges.IncidentEdge = incident1;
                        contact.LastImpulse.Feature.edges.Flipped = overlapData.Flipped;

                        ++contactPair.ContactCount;
                  }
            }

            return true;
      }

      static uint8_t FindIncidentEdge(Vector2Span vertices, Vector2 referenceNormal)
      {
            Fixed16_16 minDot = std::numeric_limits<Fixed16_16>::max();
            uint8_t incidentIndex = 0;

            for (int i = 0; i < vertices.size; ++i)
            {
                  uint8_t j = (i + 1) % vertices.size;
                  Vector2 e = vertices[j] - vertices[i];
                  Vector2 normal = e.PerpendicularInverse().Normalize();
                  Fixed16_16 dot = referenceNormal.Dot(normal);

                  if (dot < minDot)
                  {
                        minDot = dot;
                        incidentIndex = i;
                  }
            }

            return incidentIndex;
      }

      static uint8_t ClipSegmentToHalfSpace(const std::array<Vector2, 2>& clipPoints, const Vector2 n, const Fixed16_16 o, std::array<Vector2, 2>& outClipPoints)
      {
            uint8_t count = 0;
            Fixed16_16 da = n.Dot(clipPoints[0]) - o;
            Fixed16_16 db = n.Dot(clipPoints[1]) - o;

            //Keep points on or inside (<= 0)
            if (da <= Fixed16_16(0)) { outClipPoints[count++] = clipPoints[0]; }
            if (db <= Fixed16_16(0)) { outClipPoints[count++] = clipPoints[1]; }

            //Check if the points are on different sides
            if ((da < Fixed16_16(0) && db > Fixed16_16(0)) || (da > Fixed16_16(0) && db < Fixed16_16(0)))
            {
                  Fixed16_16 t = da / (da - db);
                  Vector2 p = clipPoints[0] + (clipPoints[1] - clipPoints[0]) * t;
                  outClipPoints[count++] = p;
            }

            return count;
      }

      static inline bool EvaluatePenetrationBias(const OverlapData& overlay1, bool flipped2, Fixed16_16 penetration2)
      {
            if (overlay1.Flipped && !flipped2)
            {
                  return penetration2.raw_value() < overlay1.Penetration.raw_value() + 10;
            }
            if (!overlay1.Flipped && flipped2)
            {
                  return penetration2.raw_value() + 10 < overlay1.Penetration.raw_value();
            }

            return penetration2 < overlay1.Penetration;
      }

      static inline bool BiasGreaterThan(Fixed16_16 a, Fixed16_16 b)
      {
            constexpr Fixed16_16 k_biasRelative = Fixed16_16(0, 9);
            constexpr Fixed16_16 k_biasAbsolute = Fixed16_16(1) / Fixed16_16(100);
            return a >= b * k_biasRelative + a * k_biasAbsolute;
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

      static constexpr inline bool AlmostEqual(long x, long y, long epsilon)
      {
            return (x >= y - epsilon) && (x <= y + epsilon);
      }

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
};

//todo: validate shapes with ccw