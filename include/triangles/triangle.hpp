#ifndef TRIANGLES_TRIANGLE_HPP_
#define TRIANGLES_TRIANGLE_HPP_

#include <array>
#include <stdexcept>

#include "triangles/basics.hpp"

namespace triangles {

class Triangle3D {
 public:
  Triangle3D() = default;
  Triangle3D(Point3D p1, Point3D p2, Point3D p3) {
    if (!(p1.IsValid() && p2.IsValid() && p3.IsValid())) {
      throw std::runtime_error("Non valid arguments in Triangle Ctor");
    }
    vertices_ = {p1, p2, p3};
    surface_ = {p1, p2, p3};
  }

  bool IsValid() const {
    return vertices_[0].IsValid() && vertices_[1].IsValid() &&
           vertices_[2].IsValid();
  }

  bool DoesIntersect(const Triangle3D& other) const;

 private:
  bool DoesIntersectCopl(const Triangle3D& other) const;
  bool DoesIntersectNonCopl(const Triangle3D& other) const;

  std::array<Point3D, 3> vertices_{};
  Surface surface_;
};

}  // namespace triangles

#endif  // TRIANGLES_TRIANGLE_HPP_
