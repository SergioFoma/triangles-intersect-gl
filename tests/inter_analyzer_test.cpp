#include <algorithm>
#include <array>
#include <cstdint>
#include <random>
#include <gtest/gtest.h>
#include "inter_analyzer.hpp"

namespace {
using triangles::InterAnalyzer;
using triangles::TriangleArr;
using triangles::Triangle3D;

// Independent bit-by-bit reference: x occupies bits 2, 5, ..., 29.
uint32_t ReferenceMorton(const Triangle3D& triangle, const triangles::Box& bounds) {
  const std::array<double, 3> point{
      triangle.GetMinX(), triangle.GetMinY(), triangle.GetMinZ()};
  const std::array<double, 3> low{bounds.min_.x_, bounds.min_.y_, bounds.min_.z_};
  const std::array<double, 3> high{bounds.max_.x_, bounds.max_.y_, bounds.max_.z_};
  uint32_t code = 0;
  for (size_t axis = 0; axis < 3; ++axis) {
    const double normalized = high[axis] == low[axis] ? 0 :
        (point[axis] - low[axis]) / (high[axis] - low[axis]);
    const auto coordinate = static_cast<uint32_t>(
        std::clamp(normalized * 1024, 0.0, 1023.0));
    for (unsigned bit = 0; bit < 10; ++bit) {
      code |= ((coordinate >> bit) & 1U) << (3 * bit + 2 - axis);
    }
  }
  return code;
}

auto VerticesKey(const Triangle3D& triangle) {
  const auto& a = triangle.GetFirstPoint();
  const auto& b = triangle.GetSecondPoint();
  const auto& c = triangle.GetThirdPoint();
  return std::array<double, 9>{a.x_, a.y_, a.z_, b.x_, b.y_, b.z_,
                              c.x_, c.y_, c.z_};
}

void ExpectMortonOrder(const TriangleArr& input) {
  for (bool move : {false, true}) {
    SCOPED_TRACE(move);
    const InterAnalyzer analyzer = move ? InterAnalyzer{TriangleArr(input)} :
                                          InterAnalyzer{input};
    const auto& result = analyzer.GetTriangles();
    ASSERT_EQ(result.size(), input.size());
    ASSERT_EQ(analyzer.GetInterStatuses().size(), input.size());
    if (input.empty()) continue;
    triangles::Box bounds{input.front()};
    for (const auto& triangle : input) bounds = bounds.Merge(triangles::Box{triangle});
    for (size_t i = 1; i < result.size(); ++i) {
      EXPECT_LE(ReferenceMorton(result[i - 1], bounds), ReferenceMorton(result[i], bounds));
    }
    // Equal Morton codes need not be stable, but no triangle may be lost.
    std::vector<std::array<double, 9>> before, after;
    for (const auto& triangle : input) before.push_back(VerticesKey(triangle));
    for (const auto& triangle : result) after.push_back(VerticesKey(triangle));
    std::sort(before.begin(), before.end());
    std::sort(after.begin(), after.end());
    EXPECT_EQ(before, after);
  }
}

void ExpectBruteForceStatuses(const InterAnalyzer& analyzer) {
  const auto& triangles = analyzer.GetTriangles();
  std::vector<bool> expected(triangles.size(), false);
  for (size_t i = 0; i < triangles.size(); ++i) {
    for (size_t j = i + 1; j < triangles.size(); ++j) {
      if (triangles[i].DoesIntersect(triangles[j])) expected[i] = expected[j] = true;
    }
  }
  EXPECT_EQ(analyzer.GetInterStatuses(), expected);
}

void ExpectBruteForce(const TriangleArr& input) {
  ExpectBruteForceStatuses(InterAnalyzer{input});
  ExpectBruteForceStatuses(InterAnalyzer{TriangleArr(input)});
}

TEST(InterAnalyzer, MortonOrderInterleavesAllThreeAxes) {
  TriangleArr input;
  for (int code : {7, 3, 5, 1, 6, 2, 4, 0}) {
    const double x = (code & 4) ? 4 : 0;
    const double y = (code & 2) ? 4 : 0;
    const double z = (code & 1) ? 4 : 0;
    input.emplace_back(triangles::Point3D{x, y, z},
                       triangles::Point3D{x + 1, y, z},
                       triangles::Point3D{x, y + 1, z});
  }
  const InterAnalyzer analyzer{input};
  for (size_t i = 0; i < input.size(); ++i) {
    const auto& point = analyzer.GetTriangles()[i].GetFirstPoint();
    const size_t code = (point.x_ > 0 ? 4 : 0) |
                        (point.y_ > 0 ? 2 : 0) |
                        (point.z_ > 0 ? 1 : 0);
    EXPECT_EQ(code, i);
  }
  ExpectBruteForce(input);
}

TEST(InterAnalyzer, RejectsInvalidTriangle) {
  EXPECT_THROW((InterAnalyzer{TriangleArr{Triangle3D{}}}), std::runtime_error);
}

TEST(InterAnalyzer, MortonOrderUsesMinimumCornerInsteadOfCenter) {
  const Triangle3D wide{{0, 0, 0}, {100, 0, 0}, {0, 100, 0}};
  const Triangle3D small{{1, 1, 0}, {2, 1, 0}, {1, 2, 0}};
  const InterAnalyzer analyzer{TriangleArr{small, wide}};
  EXPECT_DOUBLE_EQ(analyzer.GetTriangles().front().GetMinX(), 0);
  EXPECT_DOUBLE_EQ(analyzer.GetTriangles().front().GetMaxX(), 100);
  ExpectBruteForceStatuses(analyzer);
}

TEST(InterAnalyzer, EmptyAndSingleton) {
  ExpectMortonOrder({});
  ExpectMortonOrder({Triangle3D{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}}});
  ExpectBruteForce({});
  ExpectBruteForce({Triangle3D{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}}});
}

TEST(InterAnalyzer, MarkedTrianglesRemainAvailableAsPartners) {
  TriangleArr triangles;
  // Disjoint small triangles all intersect one large triangle.
  triangles.emplace_back(triangles::Point3D{0, 0, 0},
                         triangles::Point3D{100, 0, 0},
                         triangles::Point3D{0, 100, 0});
  for (int i = 1; i < 40; ++i) {
    const double x = i * 2;
    triangles.emplace_back(triangles::Point3D{x, 1, 0},
                           triangles::Point3D{x + 0.5, 1, 0},
                           triangles::Point3D{x, 1.5, 0});
  }
  ExpectBruteForce(triangles);
}

TEST(InterAnalyzer, EqualMortonCodes) {
  for (size_t count : {7, 8, 9, 16, 17, 65}) {
    SCOPED_TRACE(count);
    ExpectBruteForce(TriangleArr(count, Triangle3D{{0, 0, 0}, {2, 0, 0}, {0, 2, 0}}));
    ExpectMortonOrder(TriangleArr(count, Triangle3D{{0, 0, 0}, {2, 0, 0}, {0, 2, 0}}));
  }
}

TEST(InterAnalyzer, SeparatedAlongEachAxis) {
  for (size_t axis = 0; axis < 3; ++axis) {
    TriangleArr input;
    for (int i = 0; i < 100; ++i) {
      double x = axis == 0 ? i * 3 : 0;
      double y = axis == 1 ? i * 3 : 0;
      double z = axis == 2 ? i * 3 : 0;
      input.emplace_back(triangles::Point3D{x, y, z},
                         triangles::Point3D{x + 1, y, z},
                         triangles::Point3D{x, y + 1, z});
    }
    ExpectBruteForce(input);
  }
}

TEST(InterAnalyzer, ContactsAndTolerance) {
  TriangleArr input;
  for (int i = 0; i < 20; ++i) {
    double x = i * 4;
    input.emplace_back(triangles::Point3D{x, 0, 0},
                       triangles::Point3D{x + 1, 0, 0}, triangles::Point3D{x, 1, 0});
    input.emplace_back(triangles::Point3D{x + 1, 0, 0},
                       triangles::Point3D{x + 2, 0, 0}, triangles::Point3D{x + 1, 1, 0});
    input.emplace_back(triangles::Point3D{x, 0, triangles::kEps / 2},
                       triangles::Point3D{x + 1, 0, triangles::kEps / 2},
                       triangles::Point3D{x, 1, triangles::kEps / 2});
  }
  ExpectBruteForce(input);
}

TEST(InterAnalyzer, MortonOrderDoesNotPermitEarlyExitAlongX) {
  const TriangleArr input{
      Triangle3D{{0, 0, 0}, {1, 0, 0}, {0, 100, 0}},
      Triangle3D{{40, 0, 0}, {41, 0, 0}, {40, 1, 0}},
      Triangle3D{{0, 80, 0}, {0.1, 80, 0}, {0, 81, 0}},
      Triangle3D{{99, 0, 0}, {100, 0, 0}, {99, 1, 0}}};
  const InterAnalyzer analyzer{input};
  EXPECT_EQ(analyzer.GetInterStatuses(), (std::vector<bool>{true, false, true, false}));
  ExpectBruteForce(input);
  ExpectMortonOrder(input);
}

TEST(InterAnalyzer, MortonReferenceForSpatialAndFlatScenes) {
  std::mt19937 rng(67890);
  std::uniform_int_distribution<int> coordinate(-1024, 1024);
  // -1 is spatial; 0, 1, 2 fix the corresponding axis at a negative value.
  for (int flat_axis : {-1, 0, 1, 2}) {
    SCOPED_TRACE(flat_axis);
    TriangleArr input;
    for (int i = 0; i < 2048; ++i) {
      std::array<double, 3> a{double(coordinate(rng)), double(coordinate(rng)),
                              double(coordinate(rng))};
      if (flat_axis >= 0) a[flat_axis] = -17;
      auto b = a, c = a;
      b[flat_axis == 0 ? 1 : 0] += 1;
      c[flat_axis == 2 ? 1 : 2] += 1;
      input.emplace_back(triangles::Point3D{a[0], a[1], a[2]},
                         triangles::Point3D{b[0], b[1], b[2]},
                         triangles::Point3D{c[0], c[1], c[2]});
    }
    ExpectMortonOrder(input);
  }
}

TEST(InterAnalyzer, RandomScenesMatchAllPairs) {
  std::mt19937 rng(12345);
  std::uniform_real_distribution<double> coordinate(-10, 10);
  for (double size : {0.1, 2.0, 20.0}) {
    for (int scene = 0; scene < 10; ++scene) {
      SCOPED_TRACE(testing::Message() << "size=" << size << " scene=" << scene);
      TriangleArr input;
      for (int i = 0; i < 150; ++i) {
        const double x = coordinate(rng), y = coordinate(rng), z = coordinate(rng);
        auto point = [&] {
          return triangles::Point3D{x + size * coordinate(rng),
                                    y + size * coordinate(rng),
                                    z + size * coordinate(rng)};
        };
        const auto a = point(), b = point(), c = point();
        input.emplace_back(a, b, c);
      }
      ExpectBruteForce(input);
    }
  }
}
}  // namespace
