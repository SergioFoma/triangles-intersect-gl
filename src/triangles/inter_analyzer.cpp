#include <tbb/parallel_sort.h>
#include <algorithm>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include "basics.hpp"

#include "inter_analyzer.hpp"

namespace triangles {
namespace {

uint32_t expandBits(uint32_t v) {
  v = (v * 0x00010001U) & 0xFF0000FFU;
  v = (v * 0x00000101U) & 0x0F00F00FU;
  v = (v * 0x00000011U) & 0xC30C30C3U;
  v = (v * 0x00000005U) & 0x49249249U;
  return v;
}
uint32_t morton3D(double x, double y, double z) {
  x = std::min(std::max(x * 1024.0, 0.0), 1023.0);
  y = std::min(std::max(y * 1024.0, 0.0), 1023.0);
  z = std::min(std::max(z * 1024.0, 0.0), 1023.0);
  uint32_t xx = expandBits(static_cast<uint32_t>(x));
  uint32_t yy = expandBits(static_cast<uint32_t>(y));
  uint32_t zz = expandBits(static_cast<uint32_t>(z));
  return (xx * 4) + (yy * 2) + zz;
}
}  // namespace

InterAnalyzer::InterAnalyzer(const TriangleArr& triangles)
    : intersect_status_(triangles.size()), triangles_(triangles) {
  ConstructorBody();
}

InterAnalyzer::InterAnalyzer(TriangleArr&& triangles)
    : intersect_status_(triangles.size()), triangles_(std::move(triangles)) {
  ConstructorBody();
}

void InterAnalyzer::ConstructorBody() {
  if (triangles_.empty()) {
    return;
  }
  triangles::Box bounds{triangles_.front()};
  for (size_t i = 1; i < triangles_.size(); ++i) {
    bounds = bounds.Merge(triangles::Box{triangles_[i]});
  }

  const Point3D diag = bounds.max_ - bounds.min_;
  const auto get_code = [min = bounds.min_, diag](const Triangle3D& triangle) {
    const Point3D point =
        Point3D{triangle.GetMinX(), triangle.GetMinY(), triangle.GetMinZ()} -
        min;
    return morton3D(diag.x_ == 0 ? 0 : point.x_ / diag.x_,
                    diag.y_ == 0 ? 0 : point.y_ / diag.y_,
                    diag.z_ == 0 ? 0 : point.z_ / diag.z_);
  };
  const auto comparator = [get_code](const Triangle3D& first,
                                     const Triangle3D& second) {
    return get_code(first) < get_code(second);
  };

  tbb::parallel_sort(triangles_.begin(), triangles_.end(), comparator);
}

void InterAnalyzer::AnalyzeIntersection() {
}
}  // namespace triangles
