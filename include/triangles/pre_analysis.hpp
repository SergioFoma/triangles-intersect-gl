#ifndef PRE_ANALYSIS_HPP_
#define PRE_ANALYSIS_HPP_

#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_sort.h>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "glm/fwd.hpp"
#include "prog_error.hpp"
#include "triangles/basics.hpp"
#include "triangles/box.hpp"
#include "triangles/triangle.hpp"
#include "triangles_storage.hpp"

namespace triangles {
template <typename T = double>
requires std::is_floating_point_v<T> class PreAnalysis {
 public:
  explicit PreAnalysis(storage::TrianglesStorage<T>& storage)
      : storage_(storage) {
    ConstructAnalyzingTriangles();
    SortTrianglesByMorton();
    ConstructTree();
  }

  std::vector<std::pair<size_t, size_t>> GetPotentialPairs() {
    std::vector<std::pair<size_t, size_t>> potential_pairs{};
    for (size_t i = 0; i < triangles_.size(); ++i) {
      CheckIntersection(i, root_, &potential_pairs);
    }
    return potential_pairs;
  };

 private:
  struct AnalyzingTriangle {
    const Triangle3D<T>* triangle;
    Box<T> box;
    size_t original_index;
    uint32_t morton_code;
  };

  storage::TrianglesStorage<T>& storage_;  // TMP
  std::vector<AnalyzingTriangle> triangles_;

  static uint32_t expandBits(uint32_t v) {
    v = (v * 0x00010001U) & 0xFF0000FFU;
    v = (v * 0x00000101U) & 0x0F00F00FU;
    v = (v * 0x00000011U) & 0xC30C30C3U;
    v = (v * 0x00000005U) & 0x49249249U;
    return v;
  }

  static uint32_t CalculateMorton3D(T x, T y, T z) {
    x = std::min(std::max(x * 1024.0, 0.0), 1023.0);
    y = std::min(std::max(y * 1024.0, 0.0), 1023.0);
    z = std::min(std::max(z * 1024.0, 0.0), 1023.0);
    const uint32_t xx = expandBits(static_cast<uint32_t>(x));
    const uint32_t yy = expandBits(static_cast<uint32_t>(y));
    const uint32_t zz = expandBits(static_cast<uint32_t>(z));
    return (xx * 4) + (yy * 2) + zz;
  }

  void ConstructAnalyzingTriangles() {
    auto& triangles = storage_.GetTriangles();
    auto& axes = storage_.GetAxes();
    for (size_t i = 0; i < triangles.size(); ++i) {
      AnalyzingTriangle triangle{
          &triangles[i],
          Box<T>::CreateTrajectoryBox(triangles[i], axes[i]),
          i,
      };
      triangles_.push_back(std::move(triangle));
    }
  }

  void SortTrianglesByMorton() {

    triangles::Box bounds{*triangles_[0].triangle};
    for (size_t i = 0; i < triangles_.size(); ++i) {
      bounds = bounds.Merge(triangles::Box<T>{*triangles_[i].triangle});
    }

    Point3D<T> diag = bounds.max_ - bounds.min_;
    for (size_t i = 0; i < triangles_.size(); ++i) {
      Point3D<T> p = triangles_[i].triangle->GetCenter();
      T x_normalized = diag.x_ == 0 ? 0 : (p.x_ - bounds.min_.x_) / diag.x_;
      T y_normalized = diag.y_ == 0 ? 0 : (p.y_ - bounds.min_.y_) / diag.y_;
      T z_normalized = diag.z_ == 0 ? 0 : (p.z_ - bounds.min_.z_) / diag.z_;
      triangles_[i].morton_code =
          CalculateMorton3D(x_normalized, y_normalized, z_normalized);
    }

    const auto k_comparator = [](const AnalyzingTriangle& t1,
                                 const AnalyzingTriangle& t2) {
      return t1.morton_code > t2.morton_code;
    };
    tbb::parallel_sort(triangles_.begin(), triangles_.end(), k_comparator);
  }
  struct Node {
    Box<T> box;
    ssize_t node_l;
    ssize_t node_r;
    size_t l_triangle;
    size_t r_triangle;
    static constexpr ssize_t kEmptyNode = -1;
  };

  size_t root_;
  std::vector<Node> bvh_tree_;

  void ConstructTree() {
    assert(!triangles_.empty());

    for (size_t i = 0; i < triangles_.size(); ++i) {
      bvh_tree_.emplace_back(Box<T>{*triangles_[i].triangle}, Node::kEmptyNode,
                             Node::kEmptyNode, i, i + 1);
    }

    for (size_t cur_layer = 0;;) {
      const size_t start_of_next_layer = bvh_tree_.size();
      if (start_of_next_layer - cur_layer == 1)
        break;

      for (size_t box = cur_layer; box < start_of_next_layer; box += 2) {
        Node next_box{};
        next_box.node_l = static_cast<ssize_t>(box);
        next_box.l_triangle = bvh_tree_[box].l_triangle;

        if (box + 1 == start_of_next_layer) {
          next_box.node_r = -1;
          next_box.box = bvh_tree_[box].box;
          next_box.r_triangle = bvh_tree_[box].r_triangle;
        } else {
          next_box.node_r = static_cast<ssize_t>(box + 1);
          next_box.box = bvh_tree_[box].box.Merge(bvh_tree_[box + 1].box);
          next_box.r_triangle = bvh_tree_[box + 1].r_triangle;
        }
        bvh_tree_.push_back(std::move(next_box));
      }
      cur_layer = start_of_next_layer;
    }
    root_ = bvh_tree_.size() - 1;
  }

  void CheckIntersection(
      size_t triangle_index, size_t box_index,
      std::vector<std::pair<size_t, size_t>>* intersections) {
    assert(intersections);
    assert(!triangles_.empty());

    const Node& cur_box = bvh_tree_[box_index];

    if (!cur_box.box.DoesIntersect(triangles_[triangle_index].box))
      return;

    if (cur_box.node_l == Node::kEmptyNode &&
        cur_box.node_r == Node::kEmptyNode) {
      size_t triangle_orig_index = triangles_[triangle_index].original_index;
      size_t other_orig_index = triangles_[cur_box.l_triangle].original_index;
      if (triangle_orig_index <= other_orig_index)
        return;
      if (triangles_[cur_box.l_triangle].box.DoesIntersect(
              triangles_[triangle_index].box))
        intersections->emplace_back(triangle_orig_index, other_orig_index);
      return;
    }

    if (cur_box.node_r == Node::kEmptyNode) {
      assert(cur_box.node_l != Node::kEmptyNode);
      CheckIntersection(triangle_index, cur_box.node_l, intersections);
      return;
    }

    CheckIntersection(triangle_index, cur_box.node_l, intersections);
    CheckIntersection(triangle_index, cur_box.node_r, intersections);
  }
};
}  // namespace triangles

#endif
