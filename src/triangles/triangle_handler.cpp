
#include <algorithm>
#include <execution>
#include <stdexcept>

#include "triangle_handler.hpp"

namespace {

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


triangles::TriangleHandler::TriangleHandler(std::istream& in) {
  
  // failbit - format errorr
  // badbit  - system error
  in.exceptions(std::istream::failbit | std::istream::badbit);

  in >> sz_;

  if (!(kLowBound < sz_ && sz_ < kUpperBound)) {
    throw std::runtime_error("Incorrect number (N) of triangles!");
  }
  
  triangles_.reserve(sz_);

  for (unsigned int ind = 0; ind < sz_; ++ind) {
    double x = NAN, y = NAN, z = NAN;
  
    triangles_.push_back(ReadTriangle(in));
  }
}

void triangles::TriangleHandler::Sort() {
  
  std::sort(std::execution::par, triangles_.begin(), triangles_.end(), comparator_);
}
