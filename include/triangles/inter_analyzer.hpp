#ifndef INTER_ANALYZER_HPP_
#define INTER_ANALYZER_HPP_

#include <algorithm>
#include <vector>

#include "basics.hpp"
#include "box.hpp"
#include "triangle.hpp"

namespace triangles {

using TriangleArr = std::vector<triangles::Triangle3D>;
class InterAnalyzer {
 public:
  explicit InterAnalyzer(TriangleArr&& triangles);
  explicit InterAnalyzer(const TriangleArr& triangles);

  const TriangleArr& GetTriangles() const { return triangles_; }

  const std::vector<bool>& GetInterStatuses() const { return intersect_status_; }

 private:
  struct Node {
    Box box;
    ssize_t node_l;
    ssize_t node_r;
    size_t l_triangle;
    size_t r_triangle;
  };
  std::vector<bool> intersect_status_;
  std::vector<Node> boxes_;
  TriangleArr triangles_;
  size_t root_;

  void AnalyzeIntersection();
  void ConstructorBody();
  void SortTrianglesByMorton();
  void ConstructTree();
  bool DoesIntersect(size_t triangle_index, size_t box_index);

};
}  // namespace triangles

#endif
