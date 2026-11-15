#ifndef BOX_HPP_
#define BOX_HPP_

#include <algorithm>
#include <cassert>
#include <type_traits>

#include "prog_error.hpp"
#include "triangles/basics.hpp"
#include "triangles/triangle.hpp"

namespace triangles {

template <typename T>
requires std::is_floating_point_v<T> struct Box {
  Point3D<T> min_;
  Point3D<T> max_;

  Box() = default;
  Box(const Point3D<T>& min, const Point3D<T>& max) : min_{min}, max_{max} {
    if (!IsValid()) {
      prog_error::ErrorInfo inf = {
          .code = prog_error::ErrorCode::kBox,
          .err_msg = "Non valid arguments in Box Ctor",
          .func_name = "Box::Box",
      };
      throw prog_error::ProgError(std::move(inf));
    }
  }

  explicit Box(const Triangle3D<T>& triangle) {
    assert(triangle.IsValid());
    min_ = {triangle.GetMinX(), triangle.GetMinY(), triangle.GetMinZ()};
    max_ = {triangle.GetMaxX(), triangle.GetMaxY(), triangle.GetMaxZ()};
  }

  bool IsValid() const {
    return min_.IsValid() && max_.IsValid() && min_.x_ <= max_.x_ &&
           min_.y_ <= max_.y_ && min_.z_ <= max_.z_;
  }

  bool DoesIntersect(const Box& other) const {
    assert(IsValid());
    assert(other.IsValid());

    return min_.x_ <= other.max_.x_ + kEps && other.min_.x_ <= max_.x_ + kEps &&
           min_.y_ <= other.max_.y_ + kEps && other.min_.y_ <= max_.y_ + kEps &&
           min_.z_ <= other.max_.z_ + kEps && other.min_.z_ <= max_.z_ + kEps;
  }

  Box Merge(const Box& other) const {
    assert(IsValid());
    assert(other.IsValid());

    return {
        {
            std::min(min_.x_, other.min_.x_),
            std::min(min_.y_, other.min_.y_),
            std::min(min_.z_, other.min_.z_),
        },
        {
            std::max(max_.x_, other.max_.x_),
            std::max(max_.y_, other.max_.y_),
            std::max(max_.z_, other.max_.z_),
        },
    };
  }

  static Box CreateTrajectoryBox(const Triangle3D<T>& triangle,
                                 const Line<T>& axis) {
    assert(triangle.IsValid());
    assert(axis.IsValid());

    const T max_radius = std::max({axis.GetDistance(triangle.GetFirstPoint()),
                                   axis.GetDistance(triangle.GetSecondPoint()),
                                   axis.GetDistance(triangle.GetThirdPoint()),});
    const Point3D<T> first = axis.GetProjection(triangle.GetFirstPoint());
    const Point3D<T> second = axis.GetProjection(triangle.GetSecondPoint());
    const Point3D<T> third = axis.GetProjection(triangle.GetThirdPoint());

    return {{std::min({first.x_, second.x_, third.x_}) - max_radius,
             std::min({first.y_, second.y_, third.y_}) - max_radius,
             std::min({first.z_, second.z_, third.z_}) - max_radius,},
            {std::max({first.x_, second.x_, third.x_}) + max_radius,
             std::max({first.y_, second.y_, third.y_}) + max_radius,
             std::max({first.z_, second.z_, third.z_}) + max_radius,},};
  }
};

}  // namespace triangles

#endif  // BOX_HPP_
