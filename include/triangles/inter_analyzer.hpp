#ifndef INTER_ANALYZER_HPP_
#define INTER_ANALYZER_HPP_

#include <vector>

#include "triangle.hpp"
#include "basics.hpp"
#include "box.hpp"

namespace triangles {

using TriangleArr = std::vector<triangles::Triangle3D>;

class InterAnalyzer {
 public:

  explicit InterAnalyzer(TriangleArr&& triangles);
  explicit InterAnalyzer(const TriangleArr&  triangles);

  const TriangleArr& GetTriangles() const {
    return triangles_;
  }

  const std::vector<bool>& GetInterStatuses() const {
    return intersect_status_;
  }

 private:
  std::vector<bool> intersect_status_;
  TriangleArr triangles_;

  void AnalyzeIntersection();
  void ConstructorBody();
};
}

#endif
