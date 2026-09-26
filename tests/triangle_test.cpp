#include <gtest/gtest.h>

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
  const triangles::Triangle3D triangle{{0, 0, 0}, Point3D{}, {0, 1, 0}};

  EXPECT_FALSE(triangle.IsValid());
}
