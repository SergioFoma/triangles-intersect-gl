#ifndef INTER_ANALYZER_HPP_
#define INTER_ANALYZER_HPP_

#include <algorithm>
#include <vector>

#include "basics.hpp"
#include "box.hpp"
#include "triangle.hpp"
#include "basics.hpp"
#include "box.hpp"
#include "triangle.hpp"

namespace triangles {

using TriangleArr = std::vector<triangles::Triangle3D>;
class InterAnalyzer {
 public:

  explicit InterAnalyzer(const TriangleArr& triangles);

  bool DoesIntersect(size_t original_index) const;

 private:
  struct Node {
    Box box;
    ssize_t node_l;
    ssize_t node_r;
    size_t l_triangle;
    size_t r_triangle;
  };
  struct AnalyzingTriangle {
    Triangle3D triangle;
    size_t original_index;
    bool intersect_status = false;
  };

  std::vector<Node> boxes_;
  std::vector<AnalyzingTriangle> triangles_;
  size_t root_;

  void SortTrianglesByIndex();
  void AnalyzeIntersection();
  void SortTrianglesByMorton();
  void ConstructTree();
  bool CheckIntersection(size_t triangle_index, size_t box_index);

};
}  // namespace triangles

#endif
