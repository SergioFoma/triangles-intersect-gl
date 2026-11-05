#ifndef READER_HPP_
#define READER_HPP_

#include <cmath>
#include <fstream>
#include <istream>
#include <string>
#include <type_traits>
#include <utility>
#include <cstdint>

#include "prog_error.hpp"
#include "triangles/triangle.hpp"

namespace reader {

template <typename T = double>
requires std::is_floating_point_v<T>
class Reader {
 public:
  explicit Reader(const std::string& file_name)
    : file_stream_(file_name, std::ios::binary) {

    if (!file_stream_.is_open()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kReadData,
                                    "Error of opening file",
                                    "Reader::Read"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  void ReadFile(triangles::TrianglesArray<T>& triangles,
                triangles::AxesArray<T>& axes,
                std::vector<T>& speeds) {

    Read(file_stream_, triangles, axes, speeds);
  }

  triangles::TrianglesArray<T> Read(std::istream& in,
                                          triangles::TrianglesArray<T>& triangles,
                                          triangles::AxesArray<T>& axes,
                                          std::vector<T>& speeds) {
    // failbit - format error, badbit  - system error
    in.exceptions(std::istream::failbit | std::istream::badbit);

    unsigned int tmp_sz = 0 ; in >> tmp_sz;
    int64_t total_time  = 0 ; in >> total_time;
    total_time_ = static_cast<double>(total_time);

    if (!(kLowBound < tmp_sz && tmp_sz <= kUpperBound) ||
        !(triangles.size() == axes.size() && triangles.size() == speeds.size()) ||
        total_time <= kZeroSeconds) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kReadData,
                                    "Incorrect input data",
                                    "Reader::Read"};
      throw prog_error::ProgError(std::move(inf));
    }

    triangles.reserve(triangles.size() + tmp_sz);
    for (unsigned int ind = 0; ind < tmp_sz; ++ind) {
      ReadObjects(in, triangles, axes, speeds);
    }

    in.exceptions(std::ios_base::goodbit);

    float flag = 0.0F;
    in >> flag;
    if(!in.eof()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kReadData,
                                    "Incorrect number of triangles",
                                    "Reader::Read"};
      throw prog_error::ProgError(std::move(inf));
    }
    return triangles;
  }

  static triangles::TrianglesArray<T> Read(const std::string& file_name) {
    std::ifstream file(file_name, std::ios::binary);

    if (!file.is_open()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kReadData,
                                    "Error of opening file",
                                    "Reader::Read"};
      throw prog_error::ProgError(std::move(inf));
    }

    return Read(file);
  }

  double GetTotalTime() const {
    return total_time_;
  }

 private:
  static constexpr unsigned int kLowBound = 0;
  static constexpr unsigned int kUpperBound = 1'000'000;
  static constexpr int64_t kZeroSeconds = 0;

  std::ifstream file_stream_;
  double total_time_ = 0.0;

  static triangles::Point3D<T> ReadPoint(std::istream& in) {

    T x = NAN;
    T y = NAN;
    T z = NAN;

    in >> x >> y >> z;

    triangles::Point3D<T> point{x, y, z};

    return point;
  }

  static triangles::Triangle3D<T> ReadTriangle(std::istream& in) {

    triangles::Point3D<T> p_1 = ReadPoint(in);
    triangles::Point3D<T> p_2 = ReadPoint(in);
    triangles::Point3D<T> p_3 = ReadPoint(in);

    triangles::Triangle3D<T> triangle{p_1, p_2, p_3};

    return triangle;
  }

  static triangles::Axis<T> ReadAxis(std::istream& in) {

    triangles::Point3D<T> p_1 = ReadPoint(in);
    triangles::Point3D<T> p_2 = ReadPoint(in);

    triangles::Axis<T> axis{p_1, p_2};

    return axis;
  }

  static T ReadSpeed(std::istream& in) {

    T speed = NAN;

    in >> speed;

    return speed;
  }

  static void ReadObjects(std::istream& in,
                          triangles::TrianglesArray<T>& triangles,
                          triangles::AxesArray<T>& axes,
                          std::vector<T>& speeds) {

    triangles.push_back(ReadTriangle(in));
    axes.push_back(ReadAxis(in));
    speeds.push_back(ReadSpeed(in));
  }
};
} // namespace reader


#endif
