#include <gtest/gtest.h>
#include <limits>
#include <stdexcept>

#include "triangles/triangle.hpp"

namespace {

using triangles::Triangle3D;

TEST(Triangle3D, Initialize) {
  const triangles::Vertices vertices{{{4, 2, 3}, {1, 2, 3}, {2, 5, 3}}};
  const Triangle3D triangle{vertices[0], vertices[1], vertices[2]};
  EXPECT_TRUE(triangle.IsValid());
  EXPECT_DOUBLE_EQ(triangle.GetMinX(), 1);
  EXPECT_DOUBLE_EQ(triangle.GetMaxX(), 4);
  for (const auto& point : vertices) {
    EXPECT_EQ(triangle.GetPointOrientation(point), triangles::Orientation::kCoplanar);
  }
}

TEST(Triangle3D, DefaultTriangleIsInvalid) {
  EXPECT_FALSE(Triangle3D{}.IsValid());
}

TEST(Triangle3D, RejectsInvalidVertices) {
  const double infinity = std::numeric_limits<double>::infinity();
  EXPECT_THROW((Triangle3D{{0, 0, 0}, triangles::Point3D{}, {0, 1, 0}}),
               std::runtime_error);
  EXPECT_THROW((Triangle3D{{0, 0, 0}, {1, 0, 0}, {0, infinity, 0}}),
               std::runtime_error);
}

void ExpectBothOrders(const Triangle3D& a, const Triangle3D& b, bool expected) {
  EXPECT_EQ(a.DoesIntersect(b), expected) << "a.DoesIntersect(b)";
  EXPECT_EQ(b.DoesIntersect(a), expected) << "b.DoesIntersect(a)";
}

const Triangle3D kReference{{0, 0, 0}, {2, 0, 0}, {0, 2, 0}};

struct CoplanarCase {
  const char* name;
  Triangle3D second;
  bool expected;
};

const CoplanarCase kCases[] = {
    {"Identical", kReference, true},
    {"ReversedWinding", {{0, 2, 0}, {2, 0, 0}, {0, 0, 0}}, true},
    {"Overlap", {{1, -1, 0}, {3, 1, 0}, {-1, 1, 0}}, true},
    {"Contained", {{0.25, 0.25, 0}, {0.5, 0.25, 0}, {0.25, 0.5, 0}}, true},
    {"SharedEdge", {{0, 0, 0}, {2, 0, 0}, {1, -1, 0}}, true},
    {"SharedPartOfEdge", {{0.5, 0, 0}, {1.5, 0, 0}, {1, -1, 0}}, true},
    {"SharedVertex", {{2, 0, 0}, {3, 0, 0}, {2, -1, 0}}, true},
    {"VertexOnEdge", {{1, 0, 0}, {0.5, -1, 0}, {1.5, -1, 0}}, true},
    {"Disjoint", {{3, 0, 0}, {4, 0, 0}, {3, 1, 0}}, false},
    {"OnlyBoundingBoxesOverlap", {{1.5, 1.5, 0}, {3, 1.5, 0}, {1.5, 3, 0}}, false},
    {"NearMiss", {{2.000001, 0, 0}, {3, 0, 0}, {2.000001, 1, 0}}, false},
};

class CoplanarTriangleIntersection : public testing::TestWithParam<CoplanarCase> {};

TEST_P(CoplanarTriangleIntersection, BothOrders) {
  ExpectBothOrders(kReference, GetParam().second, GetParam().expected);
}

INSTANTIATE_TEST_SUITE_P(
    Geometry, CoplanarTriangleIntersection, testing::ValuesIn(kCases),
    [](const testing::TestParamInfo<CoplanarCase>& info) { return info.param.name; });

TEST(CoplanarTriangleIntersection, OffsetPlane) {
  ExpectBothOrders({{0, 0, 5}, {2, 0, 5}, {0, 2, 5}},
                   {{0.25, 0.25, 5}, {0.5, 0.25, 5}, {0.25, 0.5, 5}}, true);
}

TEST(CoplanarTriangleIntersection, VerticalPlane) {
  ExpectBothOrders({{1, 0, 0}, {1, 2, 0}, {1, 0, 2}},
                   {{1, 0.25, 0.25}, {1, 0.5, 0.25}, {1, 0.25, 0.5}}, true);
}

}  // namespace
