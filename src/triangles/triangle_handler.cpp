
#include <algorithm>
#include <execution>
#include <stdexcept>
#include "basics.hpp"

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


void triangles::TriangleHandler::ReadData(std::istream& in) {
  
  // failbit - format errorr
  // badbit  - system error
  in.exceptions(std::istream::failbit | std::istream::badbit);
  
  unsigned int tmp_sz = 0;
  in >> tmp_sz;

  if (!(kLowBound < tmp_sz && tmp_sz < kUpperBound)) {
    throw std::runtime_error("Incorrect number (N) of triangles!");
  }
  
  triangles_.reserve(tmp_sz + sz_);

  for (unsigned int ind = 0; ind < tmp_sz; ++ind) {
    double x = NAN, y = NAN, z = NAN;
  
    triangles_.push_back(ReadTriangle(in));
  }

  sz_ += tmp_sz;
}

void triangles::TriangleHandler::Sort() {
  
  std::sort(std::execution::par, triangles_.begin(), triangles_.end(), comparator_);
}

void triangles::TriangleHandler::SearchIntersection() {
  
  for (unsigned int ind = 0; ind < sz_; ++ind) {
    double min_x = triangles_[ind].GetMinX();
    double max_x = triangles_[ind].GetMaxX();

    for (unsigned int next = ind + 1; next < sz_; ++next) {
      double curr_min_x = triangles_[next].GetMinX();
      double curr_max_x = triangles_[next].GetMaxX();

      if (curr_max_x < min_x || max_x < curr_min_x) {
        break;
      }
    
      triangles_[ind].DoesIntersect(triangles_[next]);
    }
  }
}

void triangles::TriangleHandler::ClearData() {
  triangles_.clear();
  sz_ = 0;
}
