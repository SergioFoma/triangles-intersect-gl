#include <gtest/gtest.h>
#include <array>

#include "triangles/triangle.hpp"

namespace {

using triangles::Triangle3D;
using triangles::Vertices;

// z = 0, x >= 0, y >= 0, x + y <= 4. At x = 1 its slice is y in [0, 3].
const Vertices kReference{{{0, 0, 0}, {4, 0, 0}, {0, 4, 0}}};
constexpr std::array<std::array<int, 3>, 6> kOrders{{
    {0, 1, 2}, {0, 2, 1}, {1, 0, 2}, {1, 2, 0}, {2, 0, 1}, {2, 1, 0}}};

void ExpectAllOrders(const Vertices& a, const Vertices& b, bool expected) {
  for (const auto& i : kOrders) {
    for (const auto& j : kOrders) {
      SCOPED_TRACE(testing::Message() << "a: " << i[0] << i[1] << i[2]
                                     << ", b: " << j[0] << j[1] << j[2]);
      const Triangle3D first{a[i[0]], a[i[1]], a[i[2]]};
      const Triangle3D second{b[j[0]], b[j[1]], b[j[2]]};
      ASSERT_EQ(first.DoesIntersect(second), expected) << "a.DoesIntersect(b)";
      ASSERT_EQ(second.DoesIntersect(first), expected) << "b.DoesIntersect(a)";
    }
  }
}

struct IntersectionCase {
  const char* name;
  Vertices second;
  bool expected;
};

const IntersectionCase kBasicCases[] = {
    {"Crossing", {{{1, 0.5, -1}, {1, 0.5, 1}, {1, 2.5, 1}}}, true},
    {"ParallelPlanes", {{{0, 0, 1}, {4, 0, 1}, {0, 4, 1}}}, false},
    {"AbovePlane", {{{0.5, 0.5, 1}, {2, 0.5, 1}, {0.5, 2, 2}}}, false},
    {"BelowPlane", {{{0.5, 0.5, -1}, {2, 0.5, -1}, {0.5, 2, -2}}}, false},
    {"PlaneCrossesOutsideTriangle", {{{5, 0, -1}, {5, 0, 1}, {5, 1, 0}}}, false},
};

const IntersectionCase kCornerCases[] = {
    // The slices on x = 1, z = 0 overlap partially, contain one another, or miss.
    {"PartialSliceOverlap", {{{1, 2, -1}, {1, 2, 1}, {1, 6, 1}}}, true},
    {"ContainsReferenceSlice", {{{1, -1, -1}, {1, -1, 1}, {1, 9, 1}}}, true},
    {"DisjointSlicesAfter", {{{1, 4, -1}, {1, 4, 1}, {1, 6, 1}}}, false},
    {"DisjointSlicesBefore", {{{1, -3, -1}, {1, -3, 1}, {1, 1, 1}}}, false},
    // Two vertices lie on the other plane: an entire edge must be handled.
    {"SharedEdge", {{{0, 0, 0}, {4, 0, 0}, {1, 0, 2}}}, true},
    {"PartialSharedEdge", {{{2, 0, 0}, {6, 0, 0}, {2, 0, 2}}}, true},
    {"ContainedEdge", {{{1, 0, 0}, {3, 0, 0}, {1, 0, 2}}}, true},
    {"DisjointCollinearEdges", {{{5, 0, 0}, {6, 0, 0}, {5, 0, 2}}}, false},
    {"EdgeOnInterior", {{{1, 0.5, 0}, {1, 1.5, 0}, {1, 1, 2}}}, true},
    // A vertex on the plane is a contact only if it belongs to both triangles.
    {"VertexOnInterior", {{{1, 1, 0}, {1, 1, 1}, {2, 1, 1}}}, true},
    {"VertexOnEdge", {{{2, 0, 0}, {2, -1, 1}, {3, 0, 1}}}, true},
    {"SharedVertex", {{{4, 0, 0}, {5, 0, 1}, {4, 1, 1}}}, true},
    {"VertexOnPlaneOutside", {{{5, 0, 0}, {5, 1, 1}, {6, 0, 1}}}, false},
    // All three orientations differ: zero, positive, negative.
    {"VertexOnPlaneOthersStraddle", {{{1, 1, 0}, {1, 0, -1}, {1, 0, 1}}}, true},
    // y = 3 is the boundary. The +/- 1e-6 offsets are much larger than kEps.
    {"JustInsideEdge", {{{1, 2.999999, -1}, {1, 2.999999, 1}, {1, 4, 1}}}, true},
    {"EdgesTouchAtOnePoint", {{{1, 3, -1}, {1, 3, 1}, {1, 4, 1}}}, true},
    {"JustOutsideEdge", {{{1, 3.000001, -1}, {1, 3.000001, 1}, {1, 4, 1}}}, false},
    // z = 1e-5 * (x - 1) still crosses z = 0 at x = 1.
    {"NearlyParallelPlanes",
     {{{0.5, 0.5, -0.000005}, {2, 0.5, 0.00001}, {0.5, 2, -0.000005}}}, true},
};

class NonCoplanarTriangleIntersection : public testing::TestWithParam<IntersectionCase> {};
class NonCoplanarCornerCases : public testing::TestWithParam<IntersectionCase> {};

TEST_P(NonCoplanarTriangleIntersection, AllOrders) {
  ExpectAllOrders(kReference, GetParam().second, GetParam().expected);
}

TEST_P(NonCoplanarCornerCases, AllOrders) {
  ExpectAllOrders(kReference, GetParam().second, GetParam().expected);
}

auto CaseName(const testing::TestParamInfo<IntersectionCase>& info) {
  return info.param.name;
}

INSTANTIATE_TEST_SUITE_P(Geometry, NonCoplanarTriangleIntersection,
                         testing::ValuesIn(kBasicCases), CaseName);
INSTANTIATE_TEST_SUITE_P(Geometry, NonCoplanarCornerCases,
                         testing::ValuesIn(kCornerCases), CaseName);

TEST(NonCoplanarTriangleIntersection, TiltedPlanesAwayFromOrigin) {
  // A common rotation, scale and translation of an intersecting pair.
  ExpectAllOrders({{{8, -16, 32}, {12, -8, 24}, {16, -12, 40}}},
                  {{{9, -11, 33}, {13, -15, 31}, {17, -13, 35}}}, true);
}

}  // namespace
