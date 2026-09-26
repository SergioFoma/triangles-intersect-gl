#include <gtest/gtest.h>
#include <limits>

#include "triangles/basics.hpp"

TEST(Point3D, InitializesCoordinates) {
  const Point3D point{1.5, 2.5, 3.5};

  EXPECT_DOUBLE_EQ(point.x_, 1.5);
  EXPECT_DOUBLE_EQ(point.y_, 2.5);
  EXPECT_DOUBLE_EQ(point.z_, 3.5);
  EXPECT_TRUE(point.IsValid());
}

TEST(Point3D, DefaultPointIsInvalid) {
  const Point3D point;

  EXPECT_FALSE(point.IsValid());
}

TEST(Point3D, AcceptsFiniteCoordinates) {
  const Point3D point{-1.5, 0.0, 2.5};

  EXPECT_TRUE(point.IsValid());
}

TEST(Point3D, RejectsNonFiniteCoordinates) {
  const double nan = std::numeric_limits<double>::quiet_NaN();
  const double infinity = std::numeric_limits<double>::infinity();

  EXPECT_FALSE(Point3D(nan, 1.0, 2.0).IsValid());
  EXPECT_FALSE(Point3D(1.0, infinity, 2.0).IsValid());
  EXPECT_FALSE(Point3D(1.0, 2.0, -infinity).IsValid());
}

TEST(Point3D, PrintsCoordinates) {
  const Point3D point{1.5, 2.5, 3.5};

  testing::internal::CaptureStdout();
  point.Print();
  EXPECT_EQ(testing::internal::GetCapturedStdout(), "Point: 1.5 2.5 3.5");
}
