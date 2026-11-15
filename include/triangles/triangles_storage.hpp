#ifndef TRIANGLES_STORAGE_HPP_
#define TRIANGLES_STORAGE_HPP_

#include <atomic>
#include <cassert>
#include <type_traits>
#include <utility>
#include <vector>

#include "reader.hpp"
#include "shapes.hpp"
#include "triangles/triangle.hpp"

namespace storage {

template <typename T = double>
requires std::is_floating_point_v<T> class TrianglesStorage {
 public:
  explicit TrianglesStorage(reader::Reader<T>& obj_reader) {
    obj_reader.ReadFile(triangles_, axes_, speeds_);
    assert(triangles_.size() == axes_.size() && axes_.size() == speeds_.size());
    predicates_ = std::vector<std::atomic_bool>(triangles_.size());

    size_t tr_numbers = 0;
    size_t line_numbers = 0;
    size_t point_numbers = 0;

    for (auto& tr : triangles_) {
      switch (tr.GetTriangleType()) {
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
          assert(0 && "No another type");
          std::unreachable();
      }
    }

    shapes_number_ = {
        .shapes_number = tr_numbers + line_numbers + point_numbers,
        .triangles_number = tr_numbers,
        .points_number = point_numbers,
        .lines_number = line_numbers,
    };
  }

  triangles::TrianglesArray<T>& GetTriangles() { return triangles_; }

  triangles::AxesArray<T>& GetAxes() { return axes_; }

  std::vector<T>& GetSpeeds() { return speeds_; }

  shapes::ShapesNumber& GetShapesNumber() { return shapes_number_; }

  std::vector<std::atomic_bool>& GetPredicates() { return predicates_; }

 private:
  triangles::TrianglesArray<T> triangles_;
  triangles::AxesArray<T> axes_;
  std::vector<T> speeds_;
  std::vector<std::atomic_bool> predicates_;
  shapes::ShapesNumber shapes_number_;
};
}  // namespace storage

#endif
