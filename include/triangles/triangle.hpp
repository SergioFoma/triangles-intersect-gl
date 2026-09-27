#ifndef TRIANGLES_TRIANGLE_HPP_
#define TRIANGLES_TRIANGLE_HPP_

#include <array>
#include <stdexcept>

#include "triangles/basics.hpp"

namespace triangles {

class Triangle3D {
 public:
  Triangle3D() = default;
  Triangle3D(Point3D p1, Point3D p2, Point3D p3)
      : vertices_{p1, p2, p3}, surface_{p1, p2, p3} {}

  const std::array<Point3D, 3>& Vertices() const { return vertices_; }

  bool IsValid() const {
    return vertices_[0].IsValid() && vertices_[1].IsValid() &&
           vertices_[2].IsValid();
  }

  bool Intersects(const Triangle3D& other) const;
  bool IsCoplanar(const Triangle3D& other) const {
    return surface_.IsCoplanar(other.surface_);
  }

 private:
  std::array<Point3D, 3> vertices_{};
  Surface surface_;
};

}  // namespace triangles

#endif  // TRIANGLES_TRIANGLE_HPP_
