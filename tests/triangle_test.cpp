#include <gtest/gtest.h>
#include <limits>

#include "triangles/triangle.hpp"

TEST(Triangle3D, Initialize) {
  const triangles::Triangle3D triangle{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
  const auto& vertices = triangle.Vertices();

  EXPECT_DOUBLE_EQ(vertices[0].x_, 1);
  EXPECT_DOUBLE_EQ(vertices[0].y_, 2);
  EXPECT_DOUBLE_EQ(vertices[0].z_, 3);
  EXPECT_DOUBLE_EQ(vertices[1].x_, 4);
  EXPECT_DOUBLE_EQ(vertices[1].y_, 5);
  EXPECT_DOUBLE_EQ(vertices[1].z_, 6);
  EXPECT_DOUBLE_EQ(vertices[2].x_, 7);
  EXPECT_DOUBLE_EQ(vertices[2].y_, 8);
  EXPECT_DOUBLE_EQ(vertices[2].z_, 9);
}

TEST(Triangle3D, ValidTriangle) {
  const triangles::Triangle3D triangle{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};

  EXPECT_TRUE(triangle.IsValid());
}

TEST(Triangle3D, InvalidVertex) {
  const triangles::Triangle3D triangle{{0, 0, 0}, triangles::Point3D{},
                                       {0, 1, 0}};

  EXPECT_FALSE(triangle.IsValid());
}

TEST(Triangle3D, DefaultTriangleIsInvalid) {
  EXPECT_FALSE(triangles::Triangle3D{}.IsValid());
}

TEST(Triangle3D, RejectsNonFiniteVertexCoordinates) {
  const double infinity = std::numeric_limits<double>::infinity();
  const triangles::Triangle3D triangle{{0, 0, 0}, {1, 0, 0},
                                       {0, infinity, 0}};

  EXPECT_FALSE(triangle.IsValid());
}

namespace {

using triangles::Triangle3D;

void ExpectIntersectionInBothOrders(const Triangle3D& first,
                                    const Triangle3D& second,
                                    bool expected) {
  EXPECT_EQ(first.Intersects(second), expected);
  EXPECT_EQ(second.Intersects(first), expected);
}

const Triangle3D kReferenceTriangle{{0, 0, 0}, {2, 0, 0}, {0, 2, 0}};

}  // namespace

TEST(TriangleIntersection, CrossesInteriorInDifferentPlanes) {
  const Triangle3D crossing{{0.5, 0.25, -1}, {0.5, 0.25, 1},
                            {0.5, 1, 0}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, crossing, true);
}

TEST(TriangleIntersection, SeparatedParallelPlanes) {
  const Triangle3D above{{0, 0, 1}, {2, 0, 1}, {0, 2, 1}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, above, false);
}

TEST(TriangleIntersection, DisjointTrianglesInDifferentPlanes) {
  const Triangle3D distant{{3, 0, -1}, {3, 0, 1}, {3, 1, 0}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, distant, false);
}

TEST(TriangleIntersection, IntersectionSegmentEndsOnReferenceEdge) {
  const Triangle3D crossing{{1, -1, -1}, {1, 2, 1}, {1, 2, -1}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, crossing, true);
}

TEST(TriangleIntersection, SharedEdgeInDifferentPlanes) {
  const Triangle3D hinged{{0, 0, 0}, {2, 0, 0}, {1, 0, 1}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, hinged, true);
}

TEST(TriangleIntersection, SecondTriangleVertexTouchesInterior) {
  const Triangle3D touching{{0.5, 0.5, 0}, {0.5, 0.5, 1}, {1, 0.5, 1}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, touching, true);
}

TEST(TriangleIntersection, SecondTriangleVertexTouchesEdge) {
  const Triangle3D touching{{1, 0, 0}, {1, -1, 1}, {1, 1, 1}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, touching, true);
}

TEST(TriangleIntersection, FirstTriangleVertexTouchesInterior) {
  const Triangle3D touching{{1, -1, -1}, {-1, 1, -1}, {0, 0, 1}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, touching, true);
}

TEST(TriangleIntersection, SharedVertexInDifferentPlanes) {
  const Triangle3D touching{{2, 0, 0}, {3, 0, 1}, {2, 1, 1}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, touching, true);
}

TEST(TriangleIntersection, EdgesCrossAtOnePoint) {
  const Triangle3D touching{{1, 1, -1}, {1, 1, 1}, {1, 2, 0}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, touching, true);
}

TEST(TriangleIntersection, PlaneCrossesButTrianglesMiss) {
  const Triangle3D distant{{1, 1, -1}, {1, 2, 1}, {1, 3, -1}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, distant, false);
}

TEST(TriangleIntersection, AllVerticesOnOneSideOfReferencePlane) {
  const Triangle3D above{{0.25, 0.25, 1}, {1, 0.25, 1},
                         {0.25, 1, 2}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, above, false);
}

TEST(TriangleIntersection, ReversingVertexOrderDoesNotChangeResult) {
  const Triangle3D crossing{{0.5, 1, 0}, {0.5, 0.25, 1},
                            {0.5, 0.25, -1}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, crossing, true);
}

TEST(TriangleIntersection, NearMissBeyondReferenceEdge) {
  const Triangle3D distant{{1, 1.000001, -1}, {1, 1.000001, 1},
                           {1, 2, 0}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, distant, false);
}

TEST(TriangleIntersection, VertexOnPlaneOutsideReferenceTriangle) {
  const Triangle3D distant{{3, 0, 0}, {3, 1, 1}, {4, 0, 1}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, distant, false);
}

TEST(CoplanarTriangleIntersection, IdenticalTriangles) {
  ExpectIntersectionInBothOrders(kReferenceTriangle, kReferenceTriangle, true);
}

TEST(CoplanarTriangleIntersection, ReversedVertexOrder) {
  const Triangle3D reversed{{0, 2, 0}, {2, 0, 0}, {0, 0, 0}};

  EXPECT_TRUE(kReferenceTriangle.IsCoplanar(reversed));
  ExpectIntersectionInBothOrders(kReferenceTriangle, reversed, true);
}

TEST(CoplanarTriangleIntersection, PartiallyOverlappingInteriors) {
  const Triangle3D overlapping{{1, -1, 0}, {3, 1, 0}, {-1, 1, 0}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, overlapping, true);
}

TEST(CoplanarTriangleIntersection, OneTriangleInsideTheOther) {
  const Triangle3D contained{{0.25, 0.25, 0}, {0.5, 0.25, 0},
                             {0.25, 0.5, 0}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, contained, true);
}

TEST(CoplanarTriangleIntersection, SharesEntireEdge) {
  const Triangle3D adjacent{{0, 0, 0}, {2, 0, 0}, {1, -1, 0}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, adjacent, true);
}

TEST(CoplanarTriangleIntersection, SharesPartOfEdge) {
  const Triangle3D adjacent{{0.5, 0, 0}, {1.5, 0, 0}, {1, -1, 0}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, adjacent, true);
}

TEST(CoplanarTriangleIntersection, TouchesAtSharedVertex) {
  const Triangle3D touching{{2, 0, 0}, {3, 0, 0}, {2, -1, 0}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, touching, true);
}

TEST(CoplanarTriangleIntersection, VertexTouchesEdgeInterior) {
  const Triangle3D touching{{1, 0, 0}, {0.5, -1, 0}, {1.5, -1, 0}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, touching, true);
}

TEST(CoplanarTriangleIntersection, DisjointTriangles) {
  const Triangle3D distant{{3, 0, 0}, {4, 0, 0}, {3, 1, 0}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, distant, false);
}

TEST(CoplanarTriangleIntersection, BoundingBoxesOverlapButTrianglesMiss) {
  const Triangle3D distant{{1.5, 1.5, 0}, {3, 1.5, 0}, {1.5, 3, 0}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, distant, false);
}

TEST(CoplanarTriangleIntersection, NearMissBeyondEdge) {
  const Triangle3D distant{{2.000001, 0, 0}, {3, 0, 0}, {2.000001, 1, 0}};

  ExpectIntersectionInBothOrders(kReferenceTriangle, distant, false);
}

TEST(CoplanarTriangleIntersection, IntersectsInOffsetPlane) {
  const Triangle3D first{{0, 0, 5}, {2, 0, 5}, {0, 2, 5}};
  const Triangle3D second{{0.25, 0.25, 5}, {0.5, 0.25, 5},
                          {0.25, 0.5, 5}};

  EXPECT_TRUE(first.IsCoplanar(second));
  ExpectIntersectionInBothOrders(first, second, true);
}

TEST(CoplanarTriangleIntersection, IntersectsInVerticalPlane) {
  const Triangle3D first{{1, 0, 0}, {1, 2, 0}, {1, 0, 2}};
  const Triangle3D second{{1, 0.25, 0.25}, {1, 0.5, 0.25},
                          {1, 0.25, 0.5}};

  EXPECT_TRUE(first.IsCoplanar(second));
  ExpectIntersectionInBothOrders(first, second, true);
}

TEST(TriangleCoplanarity, ParallelDisplacedTrianglesAreNotCoplanar) {
  const Triangle3D above{{0, 0, 1}, {2, 0, 1}, {0, 2, 1}};

  EXPECT_FALSE(kReferenceTriangle.IsCoplanar(above));
  EXPECT_FALSE(above.IsCoplanar(kReferenceTriangle));
}

TEST(TriangleCoplanarity, ReversedWindingInOffsetPlaneIsCoplanar) {
  const Triangle3D first{{0, 0, 5}, {2, 0, 5}, {0, 2, 5}};
  const Triangle3D reversed{{0, 2, 5}, {2, 0, 5}, {0, 0, 5}};

  EXPECT_TRUE(first.IsCoplanar(reversed));
  EXPECT_TRUE(reversed.IsCoplanar(first));
}

TEST(TriangleCoplanarity, IntersectingPlanesAreNotCoplanar) {
  const Triangle3D vertical{{1, 0, -1}, {1, 0, 1}, {1, 1, 0}};

  EXPECT_FALSE(kReferenceTriangle.IsCoplanar(vertical));
  EXPECT_FALSE(vertical.IsCoplanar(kReferenceTriangle));
}
