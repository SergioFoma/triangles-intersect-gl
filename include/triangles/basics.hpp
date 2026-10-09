#ifndef BASICS_HPP_
#define BASICS_HPP_

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "prog_error.hpp"

namespace triangles {

const double kEps = 1e-9;

inline bool IsZero(double v) {
  return std::abs(v) < kEps;
}

inline bool IsEqual(double v1, double v2) {
  double max_abs = std::max(std::abs(v1), std::abs(v2));
  double min_abs = std::min(std::abs(v1), std::abs(v2));
  if (IsZero(max_abs))
    return true;
  return (max_abs - min_abs) / max_abs < kEps;
}

inline bool HasSameSign(double a, double b, double c) {
  return ((a >= 0) == (b >= 0)) && ((b >= 0) == (c >= 0));
}

struct Point3D {
  double x_{NAN}, y_{NAN}, z_{NAN};

  Point3D() = default;
  Point3D(double x, double y, double z) : x_{x}, y_{y}, z_{z} {}

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

  Point3D operator/(double div) const {
    return {x_ / div, y_ / div, z_ / div};
  };

  Point3D Cross(const Point3D& other) const {
    return {(y_ * other.z_) - (z_ * other.y_),
            (z_ * other.x_) - (x_ * other.z_),
            (x_ * other.y_) - (y_ * other.x_)};
  }

  double Dot(const Point3D& other) const {
    return (x_ * other.x_) + (y_ * other.y_) + (z_ * other.z_);
  }
};

enum class Orientation : uint8_t { kNegative, kCoplanar, kPositive };

struct Surface {
  Point3D norm_;
  double d_{NAN};

  Surface() = default;
  Surface(const Point3D& p1, const Point3D& p2, const Point3D& p3) {
    if (!(p1.IsValid() && p2.IsValid() && p3.IsValid())) {
     prog_error::ErrorInfo inf = {prog_error::ErrorCode::kBasics,
                                   "Non valid arguments in Triangle Ctor",
                                   "Surface::Surface"};
      throw prog_error::ProgError(std::move(inf));
    }
    double a = ((p2.y_ - p1.y_) * (p3.z_ - p1.z_)) -
               ((p2.z_ - p1.z_) * (p3.y_ - p1.y_));
    double b = ((p2.z_ - p1.z_) * (p3.x_ - p1.x_)) -
               ((p2.x_ - p1.x_) * (p3.z_ - p1.z_));
    double c = ((p2.x_ - p1.x_) * (p3.y_ - p1.y_)) -
               ((p2.y_ - p1.y_) * (p3.x_ - p1.x_));

    double norm = std::hypot(a, b, c);
    if (IsZero(norm)) {
      norm_ = {NAN, NAN, NAN};
      d_ = NAN;
      return;
    }
    a = -a / norm;
    b = -b / norm;
    c = -c / norm;
    double d = -(a * p1.x_) - (b * p1.y_) - (c * p1.z_);
    norm_ = {a, b, c};
    d_ = d;
  }

  bool IsValid() const { return norm_.IsValid() && std::isfinite(d_); }

  Orientation GetPointOrientation(const Point3D& point) const {
    const double distance = norm_.Dot(point) + d_;
    if (IsZero(distance / std::sqrt((point.x_ * point.x_) + (point.y_ * point.y_) +
                                    (point.z_ * point.z_)))) {
      return Orientation::kCoplanar;
    }
    return distance > 0 ? Orientation::kPositive : Orientation::kNegative;
  }
};

}  // namespace triangles

#endif  // BASICS_HPP_
