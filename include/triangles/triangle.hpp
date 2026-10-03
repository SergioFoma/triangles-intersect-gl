#ifndef TRIANGLES_TRIANGLE_HPP_
#define TRIANGLES_TRIANGLE_HPP_

#include <array>
#include <stdexcept>
#include <utility>

#include "basics.hpp"

namespace triangles {

class Triangle3D {
 public:
  Triangle3D() = default;
  Triangle3D(Point3D p1, Point3D p2, Point3D p3) {
    if (!(p1.IsValid() && p2.IsValid() && p3.IsValid())) {
      throw std::runtime_error("Non valid arguments in Triangle Ctor");
    }

    if (p1.x_ > p2.x_) {
      std::swap(p1, p2);
    }
    if (p1.x_ > p3.x_) {
      std::swap(p1, p3);
    }
    if (p2.x_> p3.x_) {
      std::swap(p2, p3);
    }

    vertices_ = {p1, p2, p3};
    surface_ = {p1, p2, p3};
  }

  bool IsValid() const {
    return vertices_[0].IsValid() && vertices_[1].IsValid() &&
           vertices_[2].IsValid();
  }

  double GetMinX() const {
    return vertices_[0].x_;
  }

  double GetMaxX() const {
    return vertices_[2].x_;
  }

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

 private:
  bool DoesIntersectCopl(const Triangle3D& other) const;
  bool DoesIntersectNonCopl(const Triangle3D& other) const;
  bool CheckIfIntesectOtherSurface(const Triangle3D& other);

  std::array<Point3D, 3> vertices_{};
  Surface surface_;
};

}  // namespace triangles

#endif  // TRIANGLES_TRIANGLE_HPP_
