#include <algorithm>
#include <array>
#include <random>
#include <gtest/gtest.h>
#include "inter_analyzer.hpp"

namespace {
using triangles::InterAnalyzer;
using triangles::TriangleArr;
using triangles::Triangle3D;

void ExpectBruteForceStatuses(const InterAnalyzer& analyzer, const TriangleArr& input) {
  std::vector<bool> expected(input.size(), false);
  for (size_t i = 0; i < input.size(); ++i) {
    for (size_t j = i + 1; j < input.size(); ++j) {
      if (input[i].DoesIntersect(input[j])) expected[i] = expected[j] = true;
    }
  }
  for (size_t i = 0; i < input.size(); ++i) {
    EXPECT_EQ(analyzer.DoesIntersect(i), expected[i]) << "original index=" << i;
  }
  EXPECT_THROW(analyzer.DoesIntersect(input.size()), std::out_of_range);
}

void ExpectBruteForce(const TriangleArr& input) {
  ExpectBruteForceStatuses(InterAnalyzer{input}, input);
  ExpectBruteForceStatuses(InterAnalyzer{TriangleArr(input)}, input);
}

TEST(InterAnalyzer, OriginalIndicesAcrossAllThreeAxes) {
  TriangleArr input;
  for (int code : {7, 3, 5, 1, 6, 2, 4, 0}) {
    const double x = (code & 4) ? 4 : 0;
    const double y = (code & 2) ? 4 : 0;
    const double z = (code & 1) ? 4 : 0;
    input.emplace_back(triangles::Point3D{x, y, z},
                       triangles::Point3D{x + 1, y, z},
                       triangles::Point3D{x, y + 1, z});
  }
  ExpectBruteForce(input);
}

TEST(InterAnalyzer, RejectsInvalidTriangle) {
  EXPECT_THROW((InterAnalyzer{TriangleArr{Triangle3D{}}}), std::runtime_error);
}

TEST(InterAnalyzer, DifferentTriangleSizesPreserveOriginalIndices) {
  const Triangle3D wide{{0, 0, 0}, {100, 0, 0}, {0, 100, 0}};
  const Triangle3D small{{1, 1, 0}, {2, 1, 0}, {1, 2, 0}};
  const InterAnalyzer analyzer{TriangleArr{small, wide}};
  ExpectBruteForceStatuses(analyzer, TriangleArr{small, wide});
}

TEST(InterAnalyzer, EmptyAndSingleton) {
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
  const std::vector<bool> expected{true, false, true, false};
  for (size_t i = 0; i < input.size(); ++i) {
    EXPECT_EQ(analyzer.DoesIntersect(i), expected[i]);
  }
  ExpectBruteForce(input);
}

TEST(InterAnalyzer, OriginalIndicesForSpatialAndFlatScenes) {
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
    ExpectBruteForce(input);
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
