#include <gtest/gtest.h>
#include "triangles/inter_analyzer.hpp"

TEST(InterAnalyzer, MarksEveryTriangleInIntersectingGroup) {
  analyzer::InterAnalyzer analyzer({
      {{0, 0, 0}, {4, 0, 0}, {0, 4, 0}},
      {{1, 0.5, -1}, {1, 0.5, 1}, {1, 2, 0}},
      {{2, 0.25, -1}, {2, 0.25, 1}, {2, 1, 0}},
      {{10, 0, 0}, {11, 0, 0}, {10, 1, 0}}});
  EXPECT_EQ(analyzer.GetInterStatuses(), (std::vector<bool>{true, true, true, false}));
}
