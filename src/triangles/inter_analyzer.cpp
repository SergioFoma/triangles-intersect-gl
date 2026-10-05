#include <algorithm>
#include <execution>
#include <stdexcept>

#include "inter_analyzer.hpp"

namespace analyzer {

void InterAnalyzer::Sort() {

  std::sort(std::execution::par, triangles_.begin(), triangles_.end(), kComparator);
}

void InterAnalyzer::AnalyzeIntersection() {

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

InterAnalyzer::InterAnalyzer(analyzer::TriangleArr triangles)
  : triangles_(std::move(triangles)) {

    size_t sz = triangles_.size();
    intersect_status_.resize(sz);

    Sort();

    AnalyzeIntersection();
}

} //namespace analyzer

