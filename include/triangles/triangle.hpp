#ifndef TRIANGLES_TRIANGLE_HPP_
#define TRIANGLES_TRIANGLE_HPP_

#include <array>
#include <stdexcept>

#include "triangles/basics.hpp"

namespace triangles {

class Triangle3D {
 public:
  Triangle3D() = default;
  Triangle3D(Point3D a, Point3D b, Point3D c) : vertices_{a, b, c} {}

  const std::array<Point3D, 3>& Vertices() const { return vertices_; }

  bool IsValid() const {
    return vertices_[0].IsValid() && vertices_[1].IsValid() &&
           vertices_[2].IsValid();
  }

 private:
  std::array<Point3D, 3> vertices_{};
};

inline bool Intersects(const Triangle3D& first, const Triangle3D& second) {
  (void)first;
  (void)second;
  throw std::runtime_error("TODO: Intersects");
}

}  // namespace triangles

#endif  // TRIANGLES_TRIANGLE_HPP_
