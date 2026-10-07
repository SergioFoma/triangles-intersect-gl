#include <fstream>

#include "read_data.hpp"

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
    throw std::runtime_error("Incorrect number (N) of triangles!");
  }

  triangles::TriangleArr triangles;
  triangles.reserve(tmp_sz);

  for (unsigned int ind = 0; ind < tmp_sz; ++ind) {
    triangles.push_back(ReadTriangle(in));
  }

  return triangles;
}

triangles::TriangleArr reader::ReadData(const std::string& file_name) {
  std::ifstream file(file_name, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("ReadData: error of opening file!");
  }

  return ReadData(file);
}
