#include <stdio.h>
#include <istream>

#include "inter_analyzer.hpp"
#include "triangles_rendering.hpp"

namespace {

  constexpr unsigned int kLowBound = 0;
  constexpr unsigned int kUpperBound = 1'000'000;

  triangles::Point3D ReadPoint(std::istream& in) {
    double x = NAN;
    double y = NAN;
    double z = NAN;

    in >> x >> y >> z;

    triangles::Point3D point(x, y, z);

    return point;
  }

  triangles::Triangle3D ReadTriangle(std::istream& in) {

    triangles::Point3D p_1 = ReadPoint(in);
    triangles::Point3D p_2 = ReadPoint(in);
    triangles::Point3D p_3 = ReadPoint(in);

    triangles::Triangle3D triangle(p_1, p_2, p_3);

    return triangle;
  }

} // namespace


TriangleArr triangles::TriangleHandler::ReadData(std::istream& in) {

  // failbit - format errorr
  // badbit  - system error
  in.exceptions(std::istream::failbit | std::istream::badbit);

  unsigned int tmp_sz = 0;
  in >> tmp_sz;

  if (!(kLowBound < tmp_sz && tmp_sz < kUpperBound)) {
    throw std::runtime_error("Incorrect number (N) of triangles!");
  }

  TriangleArr triangles;
  triangles.reserve(tmp_sz);

  for (unsigned int ind = 0; ind < tmp_sz; ++ind) {
    double x = NAN, y = NAN, z = NAN;

    triangles.push_back(ReadTriangle(ind));
  }

  return triangles;
}

int main() {

  TriangleArr triangles = ReadData(std::cin);

  InterAnalyzer analyzer(std::move(triangles));

  adapter::Adapter adapter();

  size_t triangles_number = triangles.size();
  const analyzer::TriangleArr& tr_arr = analyzer.GetTriangles();
  const std::vector<bool>& intersect_status = analyzer.GetInterStatuses();
  for (size_t ind = 0; ind < triangles_number; ++ind) {
    adapter.ConvertTriangle(tr_arr[ind], intersect_status[ind]);
  }

  adapter.Draw();
}
