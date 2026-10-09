#include <cassert>
#include <cmath>
#include <cstddef>
#include <utility>

#include "basics.hpp"
<<<<<<< HEAD
#include "prog_error.hpp"
=======
#include "triangle.hpp"
>>>>>>> 036cebd (Fix scale-dependence)

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

bool Triangle3D::DoesIntersect(const Triangle3D& other) const {
  if (!IsValid() || !other.IsValid()) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kTriangle,
                                   "Non valid triangles",
                                   "Triangle3D::DoesIntersect"};
    throw prog_error::ProgError(std::move(inf));
  }

  double metric = std::hypot(metric_, other.metric_);
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

// ============================= NON COPLANAR CASE ============================

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

double GetDeterminant4x4(const Point3D& a, const Point3D& b, const Point3D& c,
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
  Vertices vertices = vertices_;
  Vertices other_vertices = other.vertices_;

  ///////////////////////////// canonization ////////////////////////////////////

  const bool reverse_other_plane = CanonicalizeVertices(vertices, orientations);
  const bool reverse_plane =
      CanonicalizeVertices(other_vertices, other_orientations);

  if (reverse_other_plane) {
    std::swap(other_vertices[1], other_vertices[2]);
  }
  if (reverse_plane) {
    std::swap(vertices[1], vertices[2]);
  }

  ///////////////////////////////////////////////////////////////////////////////

  return GetDeterminant4x4(vertices[0], vertices[1], other_vertices[0],
                           other_vertices[1]) <= metric * kEps &&
         GetDeterminant4x4(vertices[0], vertices[2], other_vertices[2],
                           other_vertices[0]) <= metric * kEps;
}

// ============================== COPLANAR CASE ===============================

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
  for (size_t i = 0; i < 3; ++i) {
    Point3D side_vector = vertices_[(i + 1) % 3] - vertices_[i];
    Point3D normal_to_side = side_vector.Cross(surface_.norm_);
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
  return false;
}

bool Triangle3D::DoesIntersectCopl(const Triangle3D& other,
                                   double metric) const {
  return !(CheckCoplSeparation(other, metric) ||
           other.CheckCoplSeparation(*this, metric));
}

}  // namespace triangles
