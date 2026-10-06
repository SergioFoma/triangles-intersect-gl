#include <algorithm>
#include <tbb/parallel_sort.h>
#include <numeric>
#include <stdexcept>
#include "basics.hpp"

#include "inter_analyzer.hpp"

analyzer::InterAnalyzer::InterAnalyzer(const analyzer::TriangleArr& triangles)
  : triangles_(triangles) {

  ConstructorBody();
}

analyzer::InterAnalyzer::InterAnalyzer(analyzer::TriangleArr&& triangles)
  : triangles_(std::move(triangles)) {

  ConstructorBody();
}

void analyzer::InterAnalyzer::ConstructorBody() {

  size_t sz = triangles_.size();
  intersect_status_.resize(sz);

  Sort();

  AnalyzeIntersection();
}

void analyzer::InterAnalyzer::Sort() {

  tbb::parallel_sort(triangles_.begin(), triangles_.end(), kComparator);
}

void analyzer::InterAnalyzer::AnalyzeIntersection() {
  size_t sz = triangles_.size();
  std::cerr << "meow";
  for (size_t ind = 0; ind < sz; ++ind) {
    double max_x = triangles_[ind].GetMaxX() + triangles::kEps;
    double min_y = triangles_[ind].GetMinY() - triangles::kEps;
    double max_y = triangles_[ind].GetMaxY() + triangles::kEps;
    double min_z = triangles_[ind].GetMinZ() - triangles::kEps;
    double max_z = triangles_[ind].GetMaxZ() + triangles::kEps;

    for (size_t next = ind + 1; next < sz; ++next) {
      double curr_min_x = triangles_[next].GetMinX();
      if (curr_min_x > max_x) break;

      if (triangles_[next].GetMinY() > max_y ||
          triangles_[next].GetMaxY() < min_y ||
          triangles_[next].GetMinZ() > max_z ||
          triangles_[next].GetMaxZ() < min_z) {
        continue;
      }

      if (triangles_[ind].DoesIntersect(triangles_[next])) {
        intersect_status_[ind] = true;
        intersect_status_[next] = true;
      }
    }
  }

  std::cerr << "meow";
}
