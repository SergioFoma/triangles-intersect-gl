#ifndef BASICS_HPP_
#define BASICS_HPP_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "prog_error.hpp"

namespace triangles {

const double kEps = 1e-9;

inline bool IsZero(double v, double precision = 1.0F) {
  return std::abs(v) < kEps * precision;
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

  bool IsEqual(const Point3D& other, double precision = 1.0F) const {
    return IsZero(x_ - other.x_, precision) &&
           IsZero(y_ - other.y_, precision) && IsZero(z_ - other.z_, precision);
  }
};

enum class Orientation : uint8_t { kNegative, kCoplanar, kPositive };

static double GetPrecisionByVertices(const Point3D& p1, const Point3D& p2,
                                     const Point3D& p3) {
  const Point3D side1 = p2 - p1;
  const Point3D side2 = p3 - p2;
  const Point3D side3 = p1 - p3;
  return std::hypot(std::hypot(side1.x_, side1.y_, side1.z_),
                    std::hypot(side2.x_, side2.y_, side2.z_),
                    std::hypot(side3.x_, side3.y_, side3.z_));
}

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
    double metric = GetPrecisionByVertices(p1, p2, p3);

    if (IsZero(norm, metric)) {
      throw std::runtime_error(
          "Unable to construct surface by 3 coplanar points");
    }
    a = -a / norm;
    b = -b / norm;
    c = -c / norm;
    double d = -(a * p1.x_) - (b * p1.y_) - (c * p1.z_);
    norm_ = {a, b, c};
    d_ = d;
  }

  bool IsValid() const { return norm_.IsValid() && std::isfinite(d_); }

  Orientation GetPointOrientation(const Point3D& point,
                                  double precision = 1.0F) const {
    assert(point.IsValid());
    const double distance = norm_.Dot(point) + d_;
    if (IsZero(distance, precision)) {
      return Orientation::kCoplanar;
    }
    return distance > 0 ? Orientation::kPositive : Orientation::kNegative;
  }
};

struct Line {
  Point3D vec_;
  Point3D start_;

  Line() = default;
  Line(const Point3D& a, const Point3D& b) {
    if (!(a.IsValid() && b.IsValid())) {
      throw std::runtime_error("Non valid arguments in Line Ctor");
    }

    vec_ = b - a;
    if (vec_.IsEqual({0, 0, 0})) {
      throw std::runtime_error("Unable to construct line by 2 similar points");
    }
    start_ = a;
  }

  bool IsValid() const { return vec_.IsValid() && start_.IsValid(); }

  bool LiesOn(const Point3D& point) const {
    assert(IsValid());
    assert(point.IsValid());

    Point3D ap = point - start_;
    double vec_len = std::hypot(vec_.x_, vec_.y_, vec_.z_);
    Point3D normalized_vec = vec_ / vec_len;
    Point3D cross_vec_ap = ap.Cross(normalized_vec);
    double h = std::hypot(cross_vec_ap.x_, cross_vec_ap.y_, cross_vec_ap.z_);
    if (!IsZero(h, vec_len))
      return false;

    double coord_on_ab = ap.Dot(normalized_vec);

    return -kEps * vec_len <= coord_on_ab &&
           coord_on_ab - vec_len <= kEps * vec_len;
  }
};

}  // namespace triangles

#endif  // BASICS_HPP_
