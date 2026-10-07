#ifndef READ_DATA_HPP_
#define READ_DATA_HPP_

#include <istream>
#include <string>

#include "triangles/triangle.hpp"
#include "triangles/inter_analyzer.hpp"

namespace reader {
constexpr unsigned int kLowBound = 0;
constexpr unsigned int kUpperBound = 1'000'000;

triangles::Point3D ReadPoint(std::istream& in);

triangles::Triangle3D ReadTriangle(std::istream& in);

triangles::TriangleArr ReadData(std::istream& in);

triangles::TriangleArr ReadData(const std::string& file_name);
} // namespace reader

#endif
