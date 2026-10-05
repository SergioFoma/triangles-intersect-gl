#ifndef INTER_ANALYZER_HPP_
#define INTER_ANALYZER_HPP_

#include <vector>

#include "triangle.hpp"

namespace analyzer {

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

  TriangleArr triangles_;
  std::vector<bool> intersect_status_;

  static constexpr auto kComparator = [](const triangles::Triangle3D& first,
                                         const triangles::Triangle3D& second) {
      return first.GetMinX() < second.GetMinX();
  };

  void Sort();

  void AnalyzeIntersection();
  
  void ConstructorBody();
};
} // namespace analyzer

#endif
