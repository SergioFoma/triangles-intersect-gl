#ifndef READ_DATA_HPP_
#define READ_DATA_HPP_

#include <cmath>
#include <fstream>
#include <istream>
#include <string>
#include <filesystem>
#include <type_traits>
#include <utility>

#include "prog_error.hpp"
#include "triangles/triangle.hpp"
#include "triangles/inter_analyzer.hpp"

namespace reader {
constexpr unsigned int kLowBound = 0;
constexpr unsigned int kUpperBound = 1'000'000;

template <typename T = double>
requires std::is_floating_point_v<T>
inline triangles::Point3D<T> ReadPoint(std::istream& in) {
  T x = NAN;
  T y = NAN;
  T z = NAN;

  in >> x >> y >> z;

  triangles::Point3D<T> point(x, y, z);

  return point;
}

template <typename T = double>
requires std::is_floating_point_v<T>
inline triangles::Triangle3D<T> ReadTriangle(std::istream& in) {

  triangles::Point3D<T> p_1 = ReadPoint<T>(in);
  triangles::Point3D<T> p_2 = ReadPoint<T>(in);
  triangles::Point3D<T> p_3 = ReadPoint<T>(in);

  triangles::Triangle3D<T> triangle(p_1, p_2, p_3);

  return triangle;
}

template <typename T = double>
requires std::is_floating_point_v<T>
inline triangles::TriangleArray<T> ReadData(std::istream& in) {
  // failbit - format errorr
  // badbit  - system error
  in.exceptions(std::istream::failbit | std::istream::badbit);

  unsigned int tmp_sz = 0;
  in >> tmp_sz;

  if (!(reader::kLowBound < tmp_sz && tmp_sz <= reader::kUpperBound)) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kReadData,
                                   "Incorrect input data",
                                   "reader::ReadData"};
    throw prog_error::ProgError(std::move(inf));
  }

  triangles::TriangleArray<T> triangles;
  triangles.reserve(tmp_sz);

  for (unsigned int ind = 0; ind < tmp_sz; ++ind) {
    triangles.push_back(ReadTriangle<T>(in));
  }

  in.exceptions(std::ios_base::goodbit);

  float flag = 0.0F;
  in >> flag;
  if(!in.eof()) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kReadData,
                                   "Incorrect number of triangles",
                                   "reader::ReadData"};
    throw prog_error::ProgError(std::move(inf));
  }

  return triangles;
}

template <typename T = double>
requires std::is_floating_point_v<T>
inline triangles::TriangleArray<T> ReadData(const std::string& file_name) {
  std::ifstream file(file_name, std::ios::binary);

  if (!file.is_open()) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kReadData,
                                   "Error of opening file",
                                   "reader::ReadData"};
    throw prog_error::ProgError(std::move(inf));
  }

  return ReadData<T>(file);
}

inline std::filesystem::path GetAbsPath(const std::string& resource_paths) {

  std::filesystem::path resources(resource_paths);
  std::filesystem::path abs_path = std::filesystem::weakly_canonical(
                                        std::filesystem::absolute(resources)
                                    );

  if (!std::filesystem::exists(resources)) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kMain,
                                   "Error of finding resource folder",
                                   "GetAbsPath"};
    throw prog_error::ProgError(std::move(inf));
  }

  return abs_path;
}

inline std::filesystem::path GetAbsRootPath(const std::string& relative_bin_paths) {
  // .../root/Build/bin/triangle_intersect -->  .../root/Build/bin/
  // .../root/Build/bin/ --> .../root/Build
  // .../root/Build --> .../root/

  std::filesystem::path bin_path = reader::GetAbsPath(relative_bin_paths);
  std::filesystem::path root_path = bin_path.parent_path().parent_path().parent_path();

  return root_path;
}

inline std::string GetAbsMaterials(const std::string& relative_bin_path) {

  std::filesystem::path root_path = reader::GetAbsRootPath(relative_bin_path);
  std::filesystem::path materials = root_path / "materials";
  std::string materials_str = materials.string();

  return materials_str;
}

} // namespace reader

#endif
