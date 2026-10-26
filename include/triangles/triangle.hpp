#ifndef TRIANGLES_TRIANGLE_HPP_
#define TRIANGLES_TRIANGLE_HPP_

#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "basics.hpp"
#include "prog_error.hpp"

namespace triangles {

template <typename T>
using Vertices = std::array<Point3D<T>, 3>;
using VerticesOrientation = std::array<Orientation, 3>;

template<typename T = double>
requires std::is_floating_point_v<T>
class Triangle3D {
 public:
  Triangle3D() = default;
  Triangle3D(Point3D<T> p1, Point3D<T> p2, Point3D<T> p3) : vertices_{p1, p2, p3} {
    if (!IsValid()) {
      prog_error::ErrorInfo inf = {.code=prog_error::ErrorCode::kTriangle,
                                   .err_msg="Non valid arguments in Triangle Ctor",
                                   .func_name="Triangle3D::Triangle3D"};
      throw prog_error::ProgError(std::move(inf));
    }

    center_coord_ = (p1 / 3 + p2 / 3 + p3 / 3);
    metric_ = GetPrecisionByVertices(p1, p2, p3);
    type_ = CalculateTriangleType();

    switch (type_) {
      case TriangleType::kDefault:
        geometry_ = Surface<T>{p1, p2, p3};
        break;
      case TriangleType::kLine: {
        auto points = kFarthestPair<T>(p1, p2, p3);
        geometry_ = Line<T>{points.first, points.second};

        break;
      }
      case TriangleType::kPoint:
        geometry_ = Point3D<T>{center_coord_};
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

  Orientation GetPointOrientation(const Point3D<T>& point, T metric) const {
    return std::get<Surface<T>>(geometry_).GetPointOrientation(point, metric);
  }

  T GetMinX() const {
    return std::min({vertices_[0].x_, vertices_[1].x_, vertices_[2].x_});
  }
  T GetMaxX() const {
    return std::max({vertices_[0].x_, vertices_[1].x_, vertices_[2].x_});
  }

  T GetMinY() const {
    return std::min({vertices_[0].y_, vertices_[1].y_, vertices_[2].y_});
  }
  T GetMaxY() const {
    return std::max({vertices_[0].y_, vertices_[1].y_, vertices_[2].y_});
  }

  T GetMinZ() const {
    return std::min({vertices_[0].z_, vertices_[1].z_, vertices_[2].z_});
  }
  T GetMaxZ() const {
    return std::max({vertices_[0].z_, vertices_[1].z_, vertices_[2].z_});
  }

  enum class TriangleType { kDefault, kLine, kPoint };
  TriangleType GetTriangleType() const { return type_; };

  const Point3D<T>& GetCenter() const { return center_coord_; }

  T GetPrecisionBySides() const { return metric_; }

  const Point3D<T>& GetFirstPoint() const { return vertices_[0]; }
  const Point3D<T>& GetSecondPoint() const { return vertices_[1]; }
  const Point3D<T>& GetThirdPoint() const { return vertices_[2]; }

  bool DoesIntersect(const Triangle3D& other) const {
    assert(other.IsValid());
    assert(IsValid());

    T metric = std::hypot(metric_, other.metric_);

    switch (type_) {
      case TriangleType::kDefault:
        return DoesTriangleIntersect(other, metric);
      case TriangleType::kLine:
        return DoesLineIntersect(other, metric);
      case TriangleType::kPoint:
        return DoesPointIntersect(other);
      default:
        assert(0 && "No such type");
        std::unreachable();
    }
  }

  Surface<T> GetSurface() const {
    if (geometry_.index() == 0)
      return std::get<Surface<T>>(geometry_);
    return Surface<T>{};
  }

  TriangleType GetTriangleType() { return type_; }

 private:
  enum class InterCase { kNoIntersection, kCoplanar, kCanBeIntersection };

  static InterCase GetIntersectionCase(VerticesOrientation orientations) {
    if (orientations[0] == Orientation::kCoplanar &&
        orientations[1] == Orientation::kCoplanar &&
        orientations[2] == Orientation::kCoplanar)
      return InterCase::kCoplanar;

    if ((orientations[0] == orientations[1]) &&
        (orientations[1] == orientations[2]))
      return InterCase::kNoIntersection;

    return InterCase::kCanBeIntersection;
  }

  static bool CanonicalizeVertices(Vertices<T>& vertices,
                                   VerticesOrientation orientations) {
    if (orientations[1] == orientations[2]) {
      return orientations[0] == Orientation::kNegative ||
             (orientations[0] == Orientation::kCoplanar &&
              orientations[1] == Orientation::kPositive);
    }
    if (orientations[0] == orientations[2]) {
      std::swap(vertices[0], vertices[1]);
      std::swap(vertices[1], vertices[2]);
      return orientations[1] == Orientation::kNegative ||
             (orientations[1] == Orientation::kCoplanar &&
              orientations[2] == Orientation::kPositive);
    }
    if (orientations[0] == orientations[1]) {
      std::swap(vertices[0], vertices[2]);
      std::swap(vertices[1], vertices[2]);
      return orientations[2] == Orientation::kNegative ||
             (orientations[2] == Orientation::kCoplanar &&
              orientations[0] == Orientation::kPositive);
    }
    if (orientations[1] == Orientation::kPositive) {
      std::swap(vertices[0], vertices[1]);
      std::swap(vertices[1], vertices[2]);
      return false;
    }
    if (orientations[2] == Orientation::kPositive) {
      std::swap(vertices[0], vertices[2]);
      std::swap(vertices[1], vertices[2]);
      return false;
    }
    return false;
  }

  static T GetMixedProductWithOffset(const Point3D<T>& a, const Point3D<T>& b,
                                     const Point3D<T>& c, const Point3D<T>& d) {
    Point3D<T> size_1 = a - d;
    Point3D<T> size_2 = b - d;
    Point3D<T> size_3 = c - d;
    return size_1.Dot(size_2.Cross(size_3));
  }

  enum class Side { kUndefined, kPositive, kNegative };

  static Side GetSide(T dist, T metric) {
    if (IsZero(dist, metric))
      return Side::kUndefined;
    return dist > 0 ? Side::kPositive : Side::kNegative;
  }

  bool DoesIntersectCopl(const Triangle3D& other, T metric) const {
    return !(CheckCoplSeparation(other, metric) ||
             other.CheckCoplSeparation(*this, metric));
  }

  bool DoesIntersectNonCopl(const Triangle3D& other, T metric) const;
  bool CheckOtherTriangle(const Triangle3D& other,
                          VerticesOrientation orientations, T metric) const {
    assert(other.IsValid());
    assert(IsValid());

    Point3D<T> p2 = other.vertices_[0];
    Point3D<T> q2 = other.vertices_[1];
    Point3D<T> r2 = other.vertices_[2];

    VerticesOrientation other_orientations = {GetPointOrientation(p2, metric),
                                              GetPointOrientation(q2, metric),
                                              GetPointOrientation(r2, metric)};

    switch (GetIntersectionCase(other_orientations)) {
      case InterCase::kNoIntersection:
        return false;
      case InterCase::kCanBeIntersection:
        return CheckSideIntersection(other, orientations, other_orientations,
                                     metric);
      case InterCase::kCoplanar:
        return DoesIntersectCopl(other, metric);
      default:
        assert(0 && "No such case.");
        std::unreachable();
    }
  }

  bool CheckSideIntersection(const Triangle3D& other,
                             VerticesOrientation orientations,
                             VerticesOrientation other_orientations,
                             T metric) const {
    assert(other.IsValid());
    assert(IsValid());

    Vertices<T> vertices = vertices_;
    Vertices<T> other_vertices = other.vertices_;

    // ----------------------------- canonization ------------------------------

    const bool reverse_other_plane =
        CanonicalizeVertices(vertices, orientations);
    const bool reverse_plane =
        CanonicalizeVertices(other_vertices, other_orientations);

    if (reverse_other_plane) {
      std::swap(other_vertices[1], other_vertices[2]);
    }
    if (reverse_plane) {
      std::swap(vertices[1], vertices[2]);
    }

    // -------------------------------------------------------------------------

    return GetMixedProductWithOffset(vertices[0], vertices[1],
                                     other_vertices[0],
                                     other_vertices[1]) <= metric * kEps &&
           GetMixedProductWithOffset(vertices[0], vertices[2],
                                     other_vertices[2],
                                     other_vertices[0]) <= metric * kEps;
  }

  bool CheckCoplSeparation(const Triangle3D& other, T metric) const {
    assert(other.IsValid());
    assert(IsValid());

    for (size_t i = 0; i < 3; ++i) {
      Point3D<T> side_vector = vertices_[(i + 1) % 3] - vertices_[i];
      Point3D<T> normal_to_side =
          side_vector.Cross(std::get<Surface<T>>(geometry_).norm_);

      bool does_separate = false;
      for (size_t j = 0; j < 3; ++j) {
        T dist = normal_to_side.Dot(other.vertices_[j] - vertices_[i]);
        Side other_side = GetSide(dist, metric);
        if (other_side == Side::kUndefined || other_side == Side::kPositive) {
          does_separate = true;
          break;
        }
      }
      if (!does_separate)
        return true;
    }
    return false;
  }

  bool DoesTriangleIntersect(const Triangle3D& other, T metric) const {
    assert(other.IsValid());
    assert(IsValid());

    if (other.type_ != TriangleType::kDefault)
      return other.DoesIntersect(*this);

    Point3D<T> p1 = vertices_[0];
    Point3D<T> q1 = vertices_[1];
    Point3D<T> r1 = vertices_[2];
    VerticesOrientation orientations = {other.GetPointOrientation(p1, metric),
                                        other.GetPointOrientation(q1, metric),
                                        other.GetPointOrientation(r1, metric)};
    switch (GetIntersectionCase(orientations)) {
      case InterCase::kNoIntersection:
        return false;
      case InterCase::kCanBeIntersection:
        return CheckOtherTriangle(other, orientations, metric);
      case InterCase::kCoplanar:
        return DoesIntersectCopl(other, metric);
      default:
        assert(0 && "No such case.");
        std::unreachable();
    }
  }

  bool DoesLineIntersect(const Triangle3D& other, T metric) const {
    assert(other.IsValid());
    assert(IsValid());

    switch (other.type_) {
      case TriangleType::kDefault:
        return CheckLineTriangleIntersection(std::get<Line<T>>(geometry_),
                                             other, metric);
      case TriangleType::kLine:
        return std::get<Line<T>>(geometry_).CheckIntersection(
            std::get<Line<T>>(other.geometry_), metric);
      case TriangleType::kPoint:
        return std::get<Line<T>>(geometry_).LiesOn(
            std::get<Point3D<T>>(other.geometry_), metric);
      default:
        assert(0 && "No such member of the class");
        std::unreachable();
    }
  }

  bool DoesPointIntersect(const Triangle3D& other) const {
    assert(other.IsValid());
    assert(IsValid());

    switch (other.type_) {
      case TriangleType::kDefault:
        return CheckPointTriangleIntersection(std::get<Point3D<T>>(geometry_),
                                              other);
      case TriangleType::kLine:
        return std::get<Line<T>>(other.geometry_)
            .LiesOn(std::get<Point3D<T>>(geometry_), other.metric_);
      case TriangleType::kPoint:
        return std::get<Point3D<T>>(geometry_).IsEqual(
            std::get<Point3D<T>>(other.geometry_));
      default:
        assert(0 && "No such member of the class");
        std::unreachable();
    }
  }

  static bool CheckPointTriangleIntersection(const Point3D<T>& point,
                                             const Triangle3D& other) {
    assert(other.IsValid());
    assert(point.IsValid());
    const Surface<T>& surface = std::get<Surface<T>>(other.geometry_);
    T metric = other.metric_;

    if (surface.GetPointOrientation(point, metric) != Orientation::kCoplanar) {
      return false;
    }

    for (size_t i = 0; i < 3; ++i) {
      Point3D<T> side_vector =
          other.vertices_[(i + 1) % 3] - other.vertices_[i];
      T side_length =
          std::hypot(side_vector.x_, side_vector.y_, side_vector.z_);
      side_vector = side_vector / side_length;
      Point3D<T> normal_to_side = side_vector.Cross(surface.norm_);
      T dist = normal_to_side.Dot(point - other.vertices_[i]);

      if (GetSide(dist, metric) == Side::kNegative) {
        return false;
      }
    }

    return true;
  }

  static bool CheckLineTriangleIntersection(const Line<T>& line,
                                            const Triangle3D& other, T metric) {
    assert(other.IsValid());
    assert(line.IsValid());
    const Surface<T>& surface = std::get<Surface<T>>(other.geometry_);
    const Orientation a = surface.GetPointOrientation(line.a_, metric);
    const Orientation b = surface.GetPointOrientation(line.b_, metric);

    return a == Orientation::kCoplanar && b == Orientation::kCoplanar
               ? CheckLineTriangleIntersectionCopl(line, other, metric)
               : CheckLineTriangleIntersectionNonCopl(line, other, a, b);
  }

  static bool CheckLineTriangleIntersectionCopl(const Line<T>& line,
                                                const Triangle3D& other,
                                                T metric) {
    const Surface<T>& surface = std::get<Surface<T>>(other.geometry_);

    for (size_t i = 0; i < 3; ++i) {
      Point3D<T> side_vector =
          other.vertices_[(i + 1) % 3] - other.vertices_[i];
      Point3D<T> normal_to_side = side_vector.Cross(surface.norm_);
      Side first =
          GetSide(normal_to_side.Dot(line.a_ - other.vertices_[i]), metric);
      Side second =
          GetSide(normal_to_side.Dot(line.b_ - other.vertices_[i]), metric);
      if (first == Side::kNegative && second == Side::kNegative)
        return false;
    }

    Point3D<T> normal_to_line = (line.b_ - line.a_).Cross(surface.norm_);
    Side first =
        GetSide(normal_to_line.Dot(other.vertices_[0] - line.a_), metric);
    if (first == Side::kUndefined)
      return true;
    for (size_t i = 1; i < 3; ++i) {
      if (GetSide(normal_to_line.Dot(other.vertices_[i] - line.a_), metric) !=
          first)
        return true;
    }
    return false;
  }

  static bool CheckLineTriangleIntersectionNonCopl(const Line<T>& line,
                                                   const Triangle3D& other,
                                                   Orientation a,
                                                   Orientation b) {
    if (a == b)
      return false;

    const Surface<T>& surface = std::get<Surface<T>>(other.geometry_);
    Point3D<T> point = line.a_;
    if (b == Orientation::kCoplanar) {
      point = line.b_;
    } else if (a != Orientation::kCoplanar) {
      T dist_a = surface.norm_.Dot(line.a_) + surface.d_;
      T dist_b = surface.norm_.Dot(line.b_) + surface.d_;
      point = line.a_ + (line.b_ - line.a_) / ((dist_a - dist_b) / dist_a);
    }
    return CheckPointTriangleIntersection(point, other);
  }

  TriangleType CalculateTriangleType() const {

    if (vertices_[0].IsEqual(vertices_[1]) &&
        vertices_[1].IsEqual(vertices_[2]) &&
        vertices_[2].IsEqual(vertices_[1]))
      return TriangleType::kPoint;

    const Point3D<T> side1 = vertices_[1] - vertices_[0];
    const Point3D<T> side2 = vertices_[2] - vertices_[0];
    if (side1.Cross(side2).IsEqual({0, 0, 0}, metric_))
      return TriangleType::kLine;

    return TriangleType::kDefault;
  }

  TriangleType type_;
  Vertices<T> vertices_{};
  T metric_{};
  Point3D<T> center_coord_;
  using GeometryShapes = std::variant<Surface<T>, Line<T>, Point3D<T>>;
  GeometryShapes geometry_;
};

template<typename T>
requires std::is_floating_point_v<T>
using TriangleArray = std::vector<Triangle3D<T>>;

using DoubleTriangleArray = TriangleArray<double>;
}  // namespace triangles

#endif  // TRIANGLES_TRIANGLE_HPP_
