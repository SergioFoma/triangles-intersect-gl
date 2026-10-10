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
