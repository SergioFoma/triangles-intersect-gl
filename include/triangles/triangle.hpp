#ifndef TRIANGLES_TRIANGLE_HPP_
#define TRIANGLES_TRIANGLE_HPP_

#include <algorithm>
#include <array>
#include <stdexcept>

#include "basics.hpp"

namespace triangles {

using Vertices = std::array<Point3D, 3>;
using VerticesOrientation = std::array<Orientation, 3>;

class Triangle3D {
 public:
  Triangle3D() = default;
  Triangle3D(Point3D p1, Point3D p2, Point3D p3)
      : vertices_{p1, p2, p3}, surface_{p1, p2, p3} {
    if (!IsValid()) {
      throw std::runtime_error("Non valid arguments in Triangle Ctor");
    }
  }

  bool IsValid() const {
    return vertices_[0].IsValid() && vertices_[1].IsValid() &&
           vertices_[2].IsValid() && surface_.IsValid();
  }

  Orientation GetPointOrientation(const Point3D& point) const {
    return surface_.GetPointOrientation(point);
  }

  double GetMinX() const {
    return std::min({vertices_[0].x_, vertices_[1].x_, vertices_[2].x_});
  }
  double GetMaxX() const {
    return std::max({vertices_[0].x_, vertices_[1].x_, vertices_[2].x_});
  }

  double GetMinY() const {
    return std::min({vertices_[0].y_, vertices_[1].y_, vertices_[2].y_});
  }
  double GetMaxY() const {
    return std::max({vertices_[0].y_, vertices_[1].y_, vertices_[2].y_});
  }

  double GetMinZ() const {
    return std::min({vertices_[0].z_, vertices_[1].z_, vertices_[2].z_});
  }
  double GetMaxZ() const {
    return std::max({vertices_[0].z_, vertices_[1].z_, vertices_[2].z_});
  }

  Point3D GetCenter() const {
    return (vertices_[0] / 3.0) + (vertices_[1] / 3.0) + (vertices_[2] / 3.0);
  }

  const Point3D& GetFirstPoint() const { return vertices_[0]; }
  const Point3D& GetSecondPoint() const { return vertices_[1]; }
  const Point3D& GetThirdPoint() const { return vertices_[2]; }

  const Point3D& GetFirstPoint() const {
    return vertices_[0];
  }

  const Point3D& GetSecondPoint() const {
    return vertices_[1];
  }

  const Point3D& GetThirdPoint() const {
    return vertices_[2];
  }
  bool DoesIntersect(const Triangle3D& other) const;

  const Surface& GetSurface() const { return surface_; }

 private:
  bool DoesIntersectCopl(const Triangle3D& other) const;
  bool DoesIntersectNonCopl(const Triangle3D& other) const;
  bool CheckOtherTriangle(const Triangle3D& other,
                          VerticesOrientation orientations) const;
  bool CheckSideIntersection(const Triangle3D& other,
                             VerticesOrientation orientations,
                             VerticesOrientation other_orientations) const;
  bool CheckCoplSeparation(const Triangle3D& other) const;

  Vertices vertices_{};
  struct Surface surface_;
};

}  // namespace triangles

#endif  // TRIANGLES_TRIANGLE_HPP_
