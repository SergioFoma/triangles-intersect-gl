#ifndef BOX_HPP_
#define BOX_HPP_

#include <algorithm>

#include "basics.hpp"
#include "triangle.hpp"
#include "prog_error.hpp"

namespace triangles {

struct Box {
  Point3D min_;
  Point3D max_;

  Box() = default;
  Box(const Point3D& min, const Point3D& max) : min_{min}, max_{max} {
    if (!IsValid()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kBox,
                                   "Non valid arguments in Box Ctor",
                                   "Box::Box"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  explicit Box(const Triangle3D& triangle) {
    if (!triangle.IsValid()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kBox,
                                   "Non valid triangle in Box Ctor",
                                   "Box::Box"};
      throw prog_error::ProgError(std::move(inf));
    }
    min_ = {triangle.GetMinX(), triangle.GetMinY(), triangle.GetMinZ()};
    max_ = {triangle.GetMaxX(), triangle.GetMaxY(), triangle.GetMaxZ()};
  }

  bool IsValid() const {
    return min_.IsValid() && max_.IsValid() && min_.x_ <= max_.x_ &&
           min_.y_ <= max_.y_ && min_.z_ <= max_.z_;
  }

  bool DoesIntersect(const Box& other) const {
    if (!IsValid() || !other.IsValid()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kBox,
                                   "Non valid triangles",
                                   "Box::DoesIntersect"};
      throw prog_error::ProgError(std::move(inf));
    }
    return min_.x_ <= other.max_.x_ + kEps && other.min_.x_ <= max_.x_ + kEps &&
           min_.y_ <= other.max_.y_ + kEps && other.min_.y_ <= max_.y_ + kEps &&
           min_.z_ <= other.max_.z_ + kEps && other.min_.z_ <= max_.z_ + kEps;
  }

  Box Merge(const Box& other) const {
    if (!IsValid() || !other.IsValid()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kBox,
                                   "Non valid box",
                                   "Box::Merge"};
      throw prog_error::ProgError(std::move(inf));
    }
    return {{std::min(min_.x_, other.min_.x_), std::min(min_.y_, other.min_.y_),
             std::min(min_.z_, other.min_.z_)},
            {std::max(max_.x_, other.max_.x_), std::max(max_.y_, other.max_.y_),
             std::max(max_.z_, other.max_.z_)}};
  }
};

}  // namespace triangles

#endif  // BOX_HPP_
