#ifndef BASICS_HPP_
#define BASICS_HPP_

#include <cmath>
#include <cstdlib>
#include <iostream>

namespace triangles {

const double kEps = 1e-9;

struct Point3D {
  double x_{NAN}, y_{NAN}, z_{NAN};

  Point3D() = default;
  Point3D(double x, double y, double z) : x_{x}, y_{y}, z_{z} {}

  bool IsValid() const {
    return std::isfinite(x_) && std::isfinite(y_) && std::isfinite(z_);
  }
  void Print() const { std::cout << "Point: " << x_ << ' ' << y_ << ' ' << z_; }
};

struct Surface {
  double a_{NAN}, b_{NAN}, c_{NAN}, d_{NAN};

  Surface() = default;
  Surface(const Point3D& p1, const Point3D& p2, const Point3D& p3) {
    double a = ((p2.y_ - p1.y_) * (p3.z_ - p1.z_)) -
               ((p2.z_ - p1.z_) * (p3.y_ - p1.y_));
    double b = ((p2.z_ - p1.z_) * (p3.x_ - p1.x_)) -
               ((p2.x_ - p1.x_) * (p3.z_ - p1.z_));
    double c = ((p2.x_ - p1.x_) * (p3.y_ - p1.y_)) -
               ((p2.y_ - p1.y_) * (p3.x_ - p1.x_));
    double norm = std::sqrt((a * a) + (b * b) + (c * c));
    a = a / norm; b = b / norm; c = c / norm;
    double d = -(a * p1.x_) - (b * p1.y_) - (c * p1.z_);
    a_ = a; b_ = b; c_ = c; d_ = d;
  }

  bool IsCoplanar(const Surface& other) const {
    double sign =
        (a_ * other.a_) + (b_ * other.b_) + (c_ * other.c_) < 0 ? -1.0 : 1.0;
    return std::abs(a_ - (sign * other.a_)) < kEps &&
           std::abs(b_ - (sign * other.b_)) < kEps &&
           std::abs(c_ - (sign * other.c_)) < kEps &&
           std::abs(d_ - (sign * other.d_)) < kEps;
  }
};

} // namespace triangles

#endif  // BASICS_HPP_
