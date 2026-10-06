#include <algorithm>
#include <tbb/parallel_sort.h>
#include <stdexcept>

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

/*
void analyzer::InterAnalyzer::AnalyzeIntersection() {

  size_t sz = triangles_.size();
  for (size_t ind = 0; ind < sz; ++ind) {

    if (intersect_status_[ind]) {
      continue;
    }

    double min_x = triangles_[ind].GetMinX();
    double max_x = triangles_[ind].GetMaxX();

    for (size_t next = ind + 1; next < sz; ++next) {

      double curr_min_x = triangles_[next].GetMinX();
      double curr_max_x = triangles_[next].GetMaxX();

      if (max_x < curr_min_x) {
        break;
      } else if (curr_max_x < min_x ) {
        continue;
      }

      if (triangles_[ind].DoesIntersect(triangles_[next])) {
        intersect_status_[ind] = true;
        break;
      }
    }
  }
}
*/

void analyzer::InterAnalyzer::AnalyzeIntersection() {

  size_t sz = triangles_.size();
  for (size_t ind = 0; ind < sz; ++ind) {
    double min_x = triangles_[ind].GetMinX();
    double max_x = triangles_[ind].GetMaxX();

    if (intersect_status_[ind]) {
        continue;
    }

    for (size_t next = ind + 1; next < sz; ++next) {
      double curr_min_x = triangles_[next].GetMinX();
      double curr_max_x = triangles_[next].GetMaxX();

      if (max_x < curr_min_x) {
        break;
      } else if (curr_max_x < min_x ) {
        continue;
      }

      intersect_status_[ind] = triangles_[ind].DoesIntersect(triangles_[next]);
      if (intersect_status_[ind]) {
        intersect_status_[next] = true;
        break;
      }
    }
  }
}

/*
void analyzer::InterAnalyzer::AnalyzeIntersection() {

  size_t sz = triangles_.size();

  for (size_t ind = 0; ind < sz; ++ind) {
    for (size_t next = 0; next < sz; ++next) {

      if (intersect_status_[ind]) {
          break;
      }

      intersect_status_[ind] = triangles_[ind].DoesIntersect(triangles_[next]);
    }
  }
}
*/

