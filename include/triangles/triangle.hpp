#ifndef TRIANGLES_TRIANGLE_HPP_
#define TRIANGLES_TRIANGLE_HPP_

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <variant>

#include "basics.hpp"
#include "prog_error.hpp"

namespace triangles {

using Vertices = std::array<Point3D, 3>;
using VerticesOrientation = std::array<Orientation, 3>;

class Triangle3D {
 public:
  Triangle3D() = default;
  Triangle3D(Point3D p1, Point3D p2, Point3D p3) : vertices_{p1, p2, p3} {
    if (!IsValid()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kTriangle,
                                   "Non valid arguments in Triangle Ctor",
                                   "Triangle3D::Triangle3D"};
      throw prog_error::ProgError(std::move(inf));
    }

    center_coord_ = (p1 / 3 + p2 / 3 + p3 / 3);
    metric_ = GetPrecisionByVertices(p1, p2, p3);
    type_ = GetTriangleType();

    Point3D min = {GetMinX(), GetMinY(), GetMinZ()};
    Point3D max = {GetMaxX(), GetMaxY(), GetMaxZ()};

    switch (type_) {
      case TriangleType::kDefault:
        geometry_ = Surface{p1, p2, p3};
        break;
      case TriangleType::kLine:
        geometry_ = Line{min, max};
        break;
      case TriangleType::kPoint:
        geometry_ = Point3D{center_coord_};
        break;
      default:
        assert(0 && "No such member of the class");
        std::unreachable();
    }
  }

  bool IsValid() const {
    return vertices_[0].IsValid() && vertices_[1].IsValid() &&
           vertices_[2].IsValid();
  }

  Orientation GetPointOrientation(const Point3D& point, double metric) const {
    return std::get<Surface>(geometry_).GetPointOrientation(point, metric);
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

  const Point3D& GetCenter() const { return center_coord_; }

  double GetPrecisionBySides() const { return metric_; }

  const Point3D& GetFirstPoint() const { return vertices_[0]; }
  const Point3D& GetSecondPoint() const { return vertices_[1]; }
  const Point3D& GetThirdPoint() const { return vertices_[2]; }

  bool DoesIntersect(const Triangle3D& other) const;

  Surface GetSurface() const {
    if (geometry_.index() == 0)
      return std::get<Surface>(geometry_);
    return Surface{};
  }

 private:
  enum class TriangleType { kDefault, kLine, kPoint };

  bool DoesIntersectCopl(const Triangle3D& other, double metric) const;
  bool DoesIntersectNonCopl(const Triangle3D& other, double metric) const;
  bool CheckOtherTriangle(const Triangle3D& other,
                          VerticesOrientation orientations,
                          double metric) const;
  bool CheckSideIntersection(const Triangle3D& other,
                             VerticesOrientation orientations,
                             VerticesOrientation other_orientations,
                             double metric) const;
  bool CheckCoplSeparation(const Triangle3D& other, double metric) const;
  bool DoesTriangleIntersect(const Triangle3D& other, double metric) const;
  bool DoesLineIntersect(const Triangle3D& other, double metric) const;
  bool DoesPointIntersect(const Triangle3D& other) const;
  bool CheckPointTriangleIntersection(const Triangle3D& other);
  TriangleType GetTriangleType() const;

  TriangleType type_;
  Vertices vertices_{};
  double metric_{};
  Point3D center_coord_;
  using GeometryShapes = std::variant<Surface, Line, Point3D>;
  GeometryShapes geometry_;
};
}  // namespace triangles

#endif  // TRIANGLES_TRIANGLE_HPP_
