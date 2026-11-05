#ifndef TRIANGLES_STORAGE_HPP_
#define TRIANGLES_STORAGE_HPP_

#include <type_traits>
#include <utility>

#include "graphics/triangles_rendering.hpp"
#include "triangles/triangle.hpp"
#include "reader.hpp"
#include "shapes.hpp"

namespace storage {

template <typename T = double>
requires std::is_floating_point_v<T>
class TrianglesStorage {
 public:
  explicit TrianglesStorage(reader::Reader<T>& obj_reader) {
    obj_reader.ReadFile(triangles_, axes_, speeds_);

    size_t tr_numbers = 0, line_numbers = 0, point_numbers = 0;

    for (auto& tr: triangles_) {
      switch(tr.GetTriangleType()) {
        case triangles::TriangleType<T>::kDefault:
          ++tr_numbers;
          break;
        case triangles::TriangleType<T>::kLine:
          ++line_numbers;
          break;
        case triangles::TriangleType<T>::kPoint:
          ++point_numbers;
          break;
        default:
         std::unreachable();
      }
    }

    shapes_number_ = {tr_numbers + line_numbers + point_numbers,
                      tr_numbers, point_numbers, line_numbers};
  }

  triangles::TrianglesArray<T>& GetTriangles() {
    return triangles_;
  }

  triangles::AxesArray<T>& GetAxes() {
    return axes_;
  }

  std::vector<T>& GetSpeeds() {
    return speeds_;
  }

  shapes::ShapesNumber& GetShapesNumber() {
    return shapes_number_;
  }

 private:
  triangles::TrianglesArray<T> triangles_;
  triangles::AxesArray<T> axes_;
  std::vector<T> speeds_;
  shapes::ShapesNumber shapes_number_;
};
} // namespace storage





#endif
