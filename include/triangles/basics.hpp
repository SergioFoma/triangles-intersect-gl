#ifndef BASICS_HPP_
#define BASICS_HPP_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "prog_error.hpp"

namespace triangles {

const double kEps = 1e-9;

template <typename T>
requires std::is_floating_point_v<T> inline bool IsZero(T v,
                                                        T precision = 1.0F) {
  return std::abs(v) < kEps * precision;
}

template <typename T = double>
requires std::is_floating_point_v<T> struct Point3D {
  T x_{NAN}, y_{NAN}, z_{NAN};

  Point3D() = default;
  Point3D(T x, T y, T z) : x_{x}, y_{y}, z_{z} {}

  bool IsValid() const {
    return std::isfinite(x_) && std::isfinite(y_) && std::isfinite(z_);
  }
  void Print() const { std::cout << "Point: " << x_ << ' ' << y_ << ' ' << z_; }

  Point3D operator+(const Point3D& other) const {
    return {x_ + other.x_, y_ + other.y_, z_ + other.z_};
  };

  Point3D operator-(const Point3D& other) const {
    return {x_ - other.x_, y_ - other.y_, z_ - other.z_};
  };

  Point3D operator/(T div) const { return {x_ / div, y_ / div, z_ / div}; };

  Point3D Cross(const Point3D& other) const {
    return {(y_ * other.z_) - (z_ * other.y_),
            (z_ * other.x_) - (x_ * other.z_),
            (x_ * other.y_) - (y_ * other.x_)};
  }

  T Dot(const Point3D& other) const {
    return (x_ * other.x_) + (y_ * other.y_) + (z_ * other.z_);
  }

  bool IsEqual(const Point3D& other, T precision = 1.0F) const {
    return IsZero(x_ - other.x_, precision) &&
           IsZero(y_ - other.y_, precision) && IsZero(z_ - other.z_, precision);
  }

  static T Distance(const Point3D& p1, const Point3D& p2) {
    const Point3D side = p2 - p1;

    return std::hypot(side.x_, side.y_, side.z_);
  }
};

enum class Orientation : uint8_t { kNegative, kCoplanar, kPositive };

template <typename T = double>
requires std::is_floating_point_v<T> T GetPrecisionByVertices(
    const Point3D<T>& p1, const Point3D<T>& p2, const Point3D<T>& p3) {
  return std::hypot(Point3D<T>::Distance(p2, p1), Point3D<T>::Distance(p3, p2),
                    Point3D<T>::Distance(p1, p3));
}

template <typename T = double>
requires std::is_floating_point_v<T> constexpr auto kFarthestPair =
    [](const Point3D<T>& a, const Point3D<T>& b, const Point3D<T>& c) {
      const T ab = Point3D<T>::Distance(a, b);
      const T ac = Point3D<T>::Distance(a, c);
      const T bc = Point3D<T>::Distance(b, c);

      if (ab >= ac && ab >= bc)
        return std::make_pair(a, b);
      if (ac >= bc)
        return std::make_pair(a, c);
      return std::make_pair(b, c);
    };

template <typename T = double>
requires std::is_floating_point_v<T> struct Surface {
  Point3D<T> norm_;
  T d_{NAN};

  Surface() = default;
  Surface(const Point3D<T>& p1, const Point3D<T>& p2, const Point3D<T>& p3) {
    if (!(p1.IsValid() && p2.IsValid() && p3.IsValid())) {
      prog_error::ErrorInfo inf = {
          .code = prog_error::ErrorCode::kBasics,
          .err_msg = "Non valid arguments in Triangle Ctor",
          .func_name = "Surface::Surface",
      };
      throw prog_error::ProgError(std::move(inf));
    }
    T a = ((p2.y_ - p1.y_) * (p3.z_ - p1.z_)) -
          ((p2.z_ - p1.z_) * (p3.y_ - p1.y_));
    T b = ((p2.z_ - p1.z_) * (p3.x_ - p1.x_)) -
          ((p2.x_ - p1.x_) * (p3.z_ - p1.z_));
    T c = ((p2.x_ - p1.x_) * (p3.y_ - p1.y_)) -
          ((p2.y_ - p1.y_) * (p3.x_ - p1.x_));

    T norm = std::hypot(a, b, c);
    T metric = GetPrecisionByVertices(p1, p2, p3);

    if (IsZero(norm, metric)) {
      throw std::runtime_error(
          "Unable to construct surface by 3 coplanar points");
    }
    a = -a / norm;
    b = -b / norm;
    c = -c / norm;
    T d = -(a * p1.x_) - (b * p1.y_) - (c * p1.z_);
    norm_ = {a, b, c};
    d_ = d;
  }

  bool IsValid() const { return norm_.IsValid() && std::isfinite(d_); }

  Orientation GetPointOrientation(const Point3D<T>& point,
                                  T precision = 1.0F) const {
    assert(point.IsValid());
    const T distance = norm_.Dot(point) + d_;
    if (IsZero(distance, precision)) {
      return Orientation::kCoplanar;
    }
    return distance > 0 ? Orientation::kPositive : Orientation::kNegative;
  }
};

template <typename T = double>
requires std::is_floating_point_v<T> struct Line {
  Point3D<T> a_, b_;

  Line() = default;
  Line(const Point3D<T>& a, const Point3D<T>& b) : a_{a}, b_{b} {
    if (!(a.IsValid() && b.IsValid())) {
      prog_error::ErrorInfo inf = {
          .code = prog_error::ErrorCode::kBasics,
          .err_msg = "Non valid arguments in Line Ctor",
          .func_name = "Line::Line",
      };
      throw prog_error::ProgError(std::move(inf));
    }

    if ((b_ - a_).IsEqual({0, 0, 0})) {
      prog_error::ErrorInfo inf = {
          .code = prog_error::ErrorCode::kBasics,
          .err_msg = "Unable to construct line by 2 similar points",
          .func_name = "Line::Line",
      };
      throw prog_error::ProgError(std::move(inf));
    }
  }

  bool IsValid() const { return a_.IsValid() && b_.IsValid(); }

  bool LiesOn(const Point3D<T>& point, T metric) const {
    assert(IsValid());
    assert(point.IsValid());

    Point3D ap = point - a_;
    Point3D ab = b_ - a_;
    T vec_len = std::hypot(ab.x_, ab.y_, ab.z_);
    Point3D normalized_vec = ab / vec_len;
    Point3D cross_vec_ap = ap.Cross(normalized_vec);
    T h = std::hypot(cross_vec_ap.x_, cross_vec_ap.y_, cross_vec_ap.z_);
    if (!IsZero(h, vec_len))
      return false;

    T coord_on_ab = ap.Dot(normalized_vec);

    return -kEps * metric <= coord_on_ab &&
           coord_on_ab - vec_len <= kEps * metric;
  }

  bool CheckIntersection(const Line& other, T metric) const {
    assert(IsValid());
    assert(other.IsValid());

    Point3D direction = b_ - a_;
    Point3D other_direction = other.b_ - other.a_;
    direction =
        direction / std::hypot(direction.x_, direction.y_, direction.z_);
    other_direction =
        other_direction /
        std::hypot(other_direction.x_, other_direction.y_, other_direction.z_);

    Point3D normal = direction.Cross(other_direction);
    T normal_len = std::hypot(normal.x_, normal.y_, normal.z_);
    if (IsZero(normal_len)) {
      return LiesOn(other.a_, metric) || LiesOn(other.b_, metric) ||
             other.LiesOn(a_, metric) || other.LiesOn(b_, metric);
    }

    normal = normal / normal_len;
    if (!IsZero((other.a_ - a_).Dot(normal), metric)) {
      return false;
    }

    const auto crosses = [&](const Line& base, const Line& segment,
                             const Point3D<T>& base_direction) {
      Point3D perpendicular = normal.Cross(base_direction);
      T first = perpendicular.Dot(segment.a_ - base.a_);
      T second = perpendicular.Dot(segment.b_ - base.a_);
      return IsZero(first, metric) || IsZero(second, metric) ||
             ((first > 0) != (second > 0));
    };

    return crosses(*this, other, direction) &&
           crosses(other, *this, other_direction);
  }
};

}  // namespace triangles

#endif  // BASICS_HPP_
