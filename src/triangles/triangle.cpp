#include <cassert>
#include <cmath>
#include <cstddef>
#include <utility>

#include "basics.hpp"
#include "prog_error.hpp"
#include "triangle.hpp"

namespace triangles {

namespace {

enum class InterCase { kNoIntersection, kCoplanar, kCanBeIntersection };

InterCase GetIntersectionCase(VerticesOrientation orientations) {
  if (orientations[0] == Orientation::kCoplanar &&
      orientations[1] == Orientation::kCoplanar &&
      orientations[2] == Orientation::kCoplanar)
    return InterCase::kCoplanar;

  if ((orientations[0] == orientations[1]) &&
      (orientations[1] == orientations[2]))
    return InterCase::kNoIntersection;

  return InterCase::kCanBeIntersection;
}
}  // namespace

// ============================== TRIANGLE TYPE ==============================

Triangle3D::TriangleType Triangle3D::GetTriangleType() const {

  if (vertices_[0].IsEqual(vertices_[1]) &&
      vertices_[1].IsEqual(vertices_[2]) && vertices_[2].IsEqual(vertices_[1]))
    return TriangleType::kPoint;

  const Point3D side1 = vertices_[1] - vertices_[0];
  const Point3D side2 = vertices_[2] - vertices_[0];
  if (side1.Cross(side2).IsEqual({0, 0, 0}, metric_))
    return TriangleType::kLine;

  return Triangle3D::TriangleType::kDefault;
}

// ========================== TRIANGLE INTERSECTION ==========================

bool Triangle3D::DoesIntersect(const Triangle3D& other) const {
  assert(other.IsValid());
  assert(IsValid());

  double metric = std::hypot(metric_, other.metric_);

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

// ============================== TRIANGLE CASE ===============================

bool Triangle3D::DoesTriangleIntersect(const Triangle3D& other,
                                       double metric) const {
  assert(other.IsValid());
  assert(IsValid());

  if (other.type_ != TriangleType::kDefault)
    return other.DoesIntersect(*this);

  Point3D p1 = vertices_[0];
  Point3D q1 = vertices_[1];
  Point3D r1 = vertices_[2];
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

// ----------------------------- NON COPLANAR CASE ----------------------------

bool Triangle3D::CheckOtherTriangle(const Triangle3D& other,
                                    VerticesOrientation orientations,
                                    double metric) const {
  assert(other.IsValid());
  assert(IsValid());

  Point3D p2 = other.vertices_[0];
  Point3D q2 = other.vertices_[1];
  Point3D r2 = other.vertices_[2];

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

namespace {
bool CanonicalizeVertices(Vertices& vertices,
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
}  // namespace

namespace {
// [a, b, c, d] :=
//                | ax ay az 1 |
//                | bx by bz 1 |
//                | cx cy cz 1 |
//                | dx dy dz 1 |

double GetMixedProduct(const Point3D& a, const Point3D& b, const Point3D& c,
                       const Point3D& d) {
  Point3D size_1 = a - d;
  Point3D size_2 = b - d;
  Point3D size_3 = c - d;
  return size_1.Dot(size_2.Cross(size_3));
}
}  // namespace

bool Triangle3D::CheckSideIntersection(const Triangle3D& other,
                                       VerticesOrientation orientations,
                                       VerticesOrientation other_orientations,
                                       double metric) const {
  assert(other.IsValid());
  assert(IsValid());

  Vertices vertices = vertices_;
  Vertices other_vertices = other.vertices_;

  // ----------------------------- canonization ------------------------------

  const bool reverse_other_plane = CanonicalizeVertices(vertices, orientations);
  const bool reverse_plane =
      CanonicalizeVertices(other_vertices, other_orientations);

  if (reverse_other_plane) {
    std::swap(other_vertices[1], other_vertices[2]);
  }
  if (reverse_plane) {
    std::swap(vertices[1], vertices[2]);
  }

  // -------------------------------------------------------------------------

  return GetMixedProduct(vertices[0], vertices[1], other_vertices[0],
                         other_vertices[1]) <= metric * kEps &&
         GetMixedProduct(vertices[0], vertices[2], other_vertices[2],
                         other_vertices[0]) <= metric * kEps;
}

// ------------------------------ COPLANAR CASE -------------------------------

namespace {
enum class Side { kUndefined, kPositive, kNegative };

Side GetSide(double dist, double metric) {
  if (IsZero(dist, metric))
    return Side::kUndefined;
  return dist > 0 ? Side::kPositive : Side::kNegative;
}
}  // namespace

bool Triangle3D::CheckCoplSeparation(const Triangle3D& other,
                                     double metric) const {
  assert(other.IsValid());
  assert(IsValid());

  for (size_t i = 0; i < 3; ++i) {
    Point3D side_vector = vertices_[(i + 1) % 3] - vertices_[i];
    Point3D normal_to_side =
        side_vector.Cross(std::get<Surface>(geometry_).norm_);

    bool does_separate = false;
    for (size_t j = 0; j < 3; ++j) {
      double dist = normal_to_side.Dot(other.vertices_[j] - vertices_[i]);
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

bool Triangle3D::DoesIntersectCopl(const Triangle3D& other,
                                   double metric) const {
  return !(CheckCoplSeparation(other, metric) ||
           other.CheckCoplSeparation(*this, metric));
}

// ================================ LINE CASE =================================

bool Triangle3D::DoesLineIntersect(const Triangle3D& other,
                                   double metric) const {
  assert(other.IsValid());
  assert(IsValid());

  return false;
}

// ================================ POINT CASE ================================

bool Triangle3D::CheckPointTriangleIntersection(const Triangle3D& other) {
  assert(other.IsValid());
  assert(IsValid());

#ifdef six_seven
  for (size_t i = 0; i < 3; ++i) {
    Point3D side_vector = other.vertices_[(i + 1) % 3] - other.vertices_[i];
    Point3D normal_to_side =
        side_vector.Cross(std::get<Surface>(geometry_).norm_);
    const Side vert_side = Side::kPositive;

    bool does_separate = false;
    for (size_t j = 0; j < 3; ++j) {
      double dist = normal_to_side.Dot(other.vertices_[j] - vertices_[i]);
      Side other_side = GetSide(dist, metric);
      if (other_side == Side::kUndefined || other_side == vert_side) {
        does_separate = true;
        break;
      }
    }
    if (!does_separate)
      return true;
  }
#endif
  return false;
}

bool Triangle3D::DoesPointIntersect(const Triangle3D& other) const {
  assert(other.IsValid());
  assert(IsValid());

  if (other.type_ == TriangleType::kDefault) {
    bool is_coplanar = std::get<Surface>(geometry_).GetPointOrientation(
                           std::get<Point3D>(geometry_), other.metric_) ==
                       Orientation::kCoplanar;
    // return is_coplanar&&
  }
  if (other.type_ == TriangleType::kPoint) {
    return std::get<Point3D>(geometry_).IsEqual(
        std::get<Point3D>(other.geometry_));
  }
  if (other.type_ == TriangleType::kLine) {
    return std::get<Line>(geometry_).LiesOn(std::get<Point3D>(other.geometry_));
  }

  return false;
}
}  // namespace triangles
