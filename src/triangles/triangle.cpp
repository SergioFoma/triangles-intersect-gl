#include <cassert>
#include <cstddef>
#include "triangle.hpp"

#include "basics.hpp"

namespace triangles {

namespace {

enum class InterCase {
  kNoIntersection,
  kCoplanar,
  kCanBeIntersection
};

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
  assert(other.IsValid());
  assert(IsValid());

  Point3D p1 = vertices_[0]; Point3D q1 = vertices_[1]; Point3D r1 = vertices_[2];
  VerticesOrientation orientations = {other.GetPointOrientation(p1),
                                      other.GetPointOrientation(q1),
                                      other.GetPointOrientation(r1)};
  switch (GetIntersectionCase(orientations)) {
    case InterCase::kNoIntersection:
      return false;
    case InterCase::kCanBeIntersection:
      return CheckOtherTriangle(other, orientations);
    case InterCase::kCoplanar:
      return DoesIntersectCopl(other);
    default:
      assert(0 && "No such case.");
      std::unreachable();
  }
}

// ============================= NON COPLANAR CASE ============================

bool Triangle3D::CheckOtherTriangle(const Triangle3D& other,
                                    VerticesOrientation orientations) const {

  Point3D p2 = other.vertices_[0];
  Point3D q2 = other.vertices_[1];
  Point3D r2 = other.vertices_[2];

  VerticesOrientation other_orientations = {GetPointOrientation(p2),
                                            GetPointOrientation(q2),
                                            GetPointOrientation(r2)};

  switch (GetIntersectionCase(other_orientations)) {
    case InterCase::kNoIntersection:
      return false;
    case InterCase::kCanBeIntersection:
      return CheckSideIntersection(other, orientations, other_orientations);
    case InterCase::kCoplanar:
      return DoesIntersectCopl(other);
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
}

namespace {
// [a, b, c, d] :=
//                | ax ay az 1 |
//                | bx by bz 1 |
//                | cx cy cz 1 |
//                | dx dy dz 1 |

double GetDeterminant4x4(const Point3D& a, const Point3D& b, const Point3D& c,
                         const Point3D& d) {
  return ((a.x_ - d.x_) *
          ((b.y_ - d.y_) * (c.z_ - d.z_) - (b.z_ - d.z_) * (c.y_ - d.y_))) -
         ((a.y_ - d.y_) *
          ((b.x_ - d.x_) * (c.z_ - d.z_) - (b.z_ - d.z_) * (c.x_ - d.x_))) +
         ((a.z_ - d.z_) *
          ((b.x_ - d.x_) * (c.y_ - d.y_) - (b.y_ - d.y_) * (c.x_ - d.x_)));
}
}

bool Triangle3D::CheckSideIntersection(const Triangle3D& other,
                                       VerticesOrientation orientations,
                                       VerticesOrientation other_orientations) const {
  Vertices vertices = vertices_;
  Vertices other_vertices = other.vertices_;

///////////////////////////// canonization ////////////////////////////////////

  const bool reverse_other_plane = CanonicalizeVertices(vertices, orientations);
  const bool reverse_plane = CanonicalizeVertices(other_vertices, other_orientations);

  if (reverse_other_plane) {
    std::swap(other_vertices[1], other_vertices[2]);
  }
  if (reverse_plane) {
    std::swap(vertices[1], vertices[2]);
  }

///////////////////////////////////////////////////////////////////////////////

  return GetDeterminant4x4(vertices[0], vertices[1], other_vertices[0],
                           other_vertices[1]) < kEps &&
         GetDeterminant4x4(vertices[0], vertices[2], other_vertices[2],
                           other_vertices[0]) < kEps;
}

// ============================== COPLANAR CASE ===============================

namespace {
enum class Side {
  kUndefined,
  kPositive,
  kNegative
};

Side GetSide(double dist) {
  if (IsZero(dist)) return Side::kUndefined;
  return dist > 0 ? Side::kPositive : Side::kNegative;
}
} // namespace

bool Triangle3D::CheckCoplSeparation(const Triangle3D& other) const {
  for (size_t i = 0; i < 3; ++i) {
    Point3D side_vector = vertices_[(i + 1) % 3] - vertices_[i];
    Point3D normal_to_side = side_vector.Cross(surface_.norm_);
    const Side vert_side = Side::kPositive;

    bool does_separate = false;
    for (size_t j = 0; j < 3; ++j) {
      double dist = normal_to_side.Dot(other.vertices_[j] - vertices_[i]);
      Side other_side = GetSide(dist);
      if (other_side == Side::kUndefined || other_side == vert_side) {
        does_separate = true;
        break;
      }
    }
    if (!does_separate) return true;
  }
  return false;
}

bool Triangle3D::DoesIntersectCopl(const Triangle3D& other) const {
  return !(CheckCoplSeparation(other) || other.CheckCoplSeparation(*this));
}

}  // namespace triangles
