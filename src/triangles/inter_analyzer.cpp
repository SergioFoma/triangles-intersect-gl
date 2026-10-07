#include <tbb/parallel_sort.h>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <utility>
#include "basics.hpp"
#include "triangle.hpp"

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

void InterAnalyzer::SortTrianglesByMorton() {
  assert(!triangles_.empty());

  triangles::Box bounds{triangles_.front()};
  for (size_t i = 1; i < triangles_.size(); ++i) {
    bounds = bounds.Merge(triangles::Box{triangles_[i]});
  }

  const auto k_get_key = [bounds](const Point3D& p) {
    Point3D diag = bounds.max_ - bounds.min_;
    double x_normalized = diag.x_ == 0 ? 0 : (p.x_ - bounds.min_.x_) / diag.x_;
    double y_normalized = diag.y_ == 0 ? 0 : (p.y_ - bounds.min_.y_) / diag.y_;
    double z_normalized = diag.z_ == 0 ? 0 : (p.z_ - bounds.min_.z_) / diag.z_;

    return morton3D(x_normalized, y_normalized, z_normalized);
  };
  const auto k_comparator = [k_get_key](const Triangle3D& t1,
                                        const Triangle3D& t2) {
    return k_get_key(t1.GetCenter()) > k_get_key(t2.GetCenter());
  };
  tbb::parallel_sort(triangles_.begin(), triangles_.end(), k_comparator);
}

void InterAnalyzer::ConstructTree() {
  if (triangles_.empty()) {
    return;
  }

  for (size_t i = 0; i < triangles_.size(); ++i) {
    boxes_.emplace_back(Box{triangles_[i]}, -1, -1, i, i + 1);
  }

  for (size_t cur_layer = 0;;) {
    const size_t start_of_next_layer = boxes_.size();
    if (start_of_next_layer - cur_layer == 1)
      break;

    for (size_t box = cur_layer; box < start_of_next_layer; box += 2) {
      Node next_box{};
      next_box.node_l = static_cast<ssize_t>(box);
      next_box.l_triangle = boxes_[box].l_triangle;

      if (box + 1 == start_of_next_layer) {
        next_box.node_r = -1;
        next_box.box = boxes_[box].box;
        next_box.r_triangle = boxes_[box].r_triangle;
      } else {
        next_box.node_r = static_cast<ssize_t>(box + 1);
        next_box.box = boxes_[box].box.Merge(boxes_[box + 1].box);
        next_box.r_triangle = boxes_[box + 1].r_triangle;
      }
      boxes_.push_back(std::move(next_box));
    }
    cur_layer = start_of_next_layer;
  }
  root_ = boxes_.size() - 1;
}

void InterAnalyzer::ConstructorBody() {
  if (triangles_.empty()) {
    return;
  }
  SortTrianglesByMorton();
  ConstructTree();
  AnalyzeIntersection();
}

bool InterAnalyzer::DoesIntersect(size_t triangle_index, size_t box_index) {
  assert(!triangles_.empty());
  const Node& cur_box = boxes_[box_index];
  if (cur_box.node_l == -1 && cur_box.node_r == -1) {
    if (cur_box.l_triangle == triangle_index &&
        cur_box.l_triangle + 1 == cur_box.r_triangle)
      return false;  // the same triangle
    if (triangles_[cur_box.l_triangle].DoesIntersect(
            triangles_[triangle_index])) {
      intersect_status_[cur_box.l_triangle] = true;
      return true;
    }
    return false;
  }

  if (cur_box.node_r == -1) {
    return DoesIntersect(triangle_index, cur_box.node_l);
  }

  if (!cur_box.box.DoesIntersect(boxes_[triangle_index].box))
    return false;

  return DoesIntersect(triangle_index, cur_box.node_l) ||
         DoesIntersect(triangle_index, cur_box.node_r);
}

void InterAnalyzer::AnalyzeIntersection() {
  for (size_t i = 0; i < triangles_.size(); ++i) {
    intersect_status_[i] = DoesIntersect(i, root_);
  }
}
}  // namespace triangles
