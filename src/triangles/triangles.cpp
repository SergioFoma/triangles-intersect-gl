#include "triangle.hpp"

#include "basics.hpp"

namespace triangles {

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
}  // namespace

bool Triangle3D::DoesIntersect(const Triangle3D& other) const {
  if (IsCoplanar(other)) {
    return DoesIntersectCopl(other);
  } else {
    return DoesIntersectNonCopl(other);
  }
}

// ============================= NON COPLANAR CASE ============================

bool Triangle3D::DoesIntersectNonCopl(const Triangle3D& other) {

  return true;
}

// ============================== COPLANAR CASE ===============================

bool Triangle3D::DoesIntersectCopl(const Triangle3D& other) {

  return true;
}

}  // namespace triangles
