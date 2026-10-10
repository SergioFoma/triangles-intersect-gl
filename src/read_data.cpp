#include <filesystem>
#include <fstream>

#include "read_data.hpp"
#include "prog_error.hpp"

triangles::Point3D reader::ReadPoint(std::istream& in) {
  double x = NAN;
  double y = NAN;
  double z = NAN;

  in >> x >> y >> z;

  triangles::Point3D point(x, y, z);

  return point;
}

triangles::Triangle3D reader::ReadTriangle(std::istream& in) {

  triangles::Point3D p_1 = ReadPoint(in);
  triangles::Point3D p_2 = ReadPoint(in);
  triangles::Point3D p_3 = ReadPoint(in);

  triangles::Triangle3D triangle(p_1, p_2, p_3);

  return triangle;
}

triangles::TriangleArr reader::ReadData(std::istream& in) {
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

  triangles::TriangleArr triangles;
  triangles.reserve(tmp_sz);

  for (unsigned int ind = 0; ind < tmp_sz; ++ind) {
    triangles.push_back(ReadTriangle(in));
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

triangles::TriangleArr reader::ReadData(const std::string& file_name) {
  std::ifstream file(file_name, std::ios::binary);

  if (!file.is_open()) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kReadData,
                                   "Error of opening file",
                                   "reader::ReadData"};
    throw prog_error::ProgError(std::move(inf));
  }

  return ReadData(file);
}

std::filesystem::path reader::GetAbsPath(const std::string& resource_paths) {

  std::filesystem::path resources(resource_paths);
   if (!std::filesystem::exists(resources)) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kMain,
                                   "Error of finding resource folder",
                                   "GetAbsPath"};
    throw prog_error::ProgError(std::move(inf));
  }

  std::filesystem::path current_wrc = std::filesystem::current_path();
  std::filesystem::path abs_path = std::filesystem::relative(resources, current_wrc);

  return abs_path;
}

std::filesystem::path reader::GetAbsRootPath(const std::string& relative_bin_paths) {
  // .../root/Build/bin/triangle_intersect -->  .../root/Build/bin/
  // .../root/Build/bin/ --> .../root/Build
  // .../root/Build --> .../root/

  std::filesystem::path bin_path = reader::GetAbsPath(relative_bin_paths);
  std::filesystem::path root_path = bin_path.parent_path().parent_path().parent_path();

  return root_path;
}

std::string reader::GetAbsMaterials(const std::string& relative_bin_path) {

  std::filesystem::path root_path = reader::GetAbsRootPath(relative_bin_path);
  std::filesystem::path materials = root_path / "materials";
  std::string materials_str = materials.string();

  return materials_str;
}
