#ifndef BASICS_HPP_
#define BASICS_HPP_

#include <cmath>
#include <iostream>
struct Point3D {
  double x_{NAN}, y_{NAN}, z_{NAN};

  Point3D() = default;
  Point3D(double x, double y, double z) : x_{x}, y_{y}, z_{z} {}

  bool IsValid() const {
    return std::isfinite(x_) && std::isfinite(y_) && std::isfinite(z_);
  }
  void Print() const { std::cout << "Point: " << x_ << ' ' << y_ << ' ' << z_; }
};

struct Line {
  double x{NAN}, y{NAN}, z{NAN};

  bool IsValid() const;
  void Print() const;
};

#endif  // BASICS_HPP_
