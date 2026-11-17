#ifndef TRIANGLES_STORAGE_HPP_
#define TRIANGLES_STORAGE_HPP_

#include <atomic>
#include <cassert>
#include <concepts>
#include <type_traits>
#include <utility>
#include <vector>

#include "reader.hpp"
#include "shapes.hpp"
#include "triangles/triangle.hpp"

namespace triangles {

template <std::floating_point T>
class StorageUpdater;



template <std::floating_point T = double>
class TrianglesStorage {
 public:
  explicit TrianglesStorage(reader::Reader<T>& obj_reader) {
    obj_reader.ReadFile(initial_pos_, axes_, speeds_);
    assert(initial_pos_.size() == axes_.size() &&
           axes_.size() == speeds_.size());
    updater_ctx_.predicates =
        std::vector<std::atomic_bool>(initial_pos_.size());
    updater_ctx_.current_pos = initial_pos_;

    size_t tr_numbers = 0;
    size_t line_numbers = 0;
    size_t point_numbers = 0;

    for (const auto& tr : initial_pos_) {
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


  struct UpdaterContext {
    TrianglesArray<T> current_pos;
    std::vector<std::atomic_bool> predicates;
    std::vector<std::pair<size_t, size_t>> potential_pairs;
  };

  void AcceptExternal(StorageUpdater<T>& updater) {
    updater.ChangeStorage(*this, &updater_ctx_);
  }

  bool IsValid() const {
    const size_t size = initial_pos_.size();
    return updater_ctx_.current_pos.size() == size && updater_ctx_.predicates.size() == size &&
           axes_.size() == size && speeds_.size() == size &&
           shapes_number_.shapes_number == size &&
           shapes_number_.triangles_number + shapes_number_.points_number +
                   shapes_number_.lines_number ==
               size;
  }

  bool IsEmpty() const {
    assert(IsValid() && "Not valid storage");
    return initial_pos_.empty();
  }

  const triangles::TrianglesArray<T>& GetCurrentPos() const {
    assert(IsValid() && "Not valid storage");
    return updater_ctx_.current_pos;
  }
  const triangles::TrianglesArray<T>& GetInitialPos() const {
    assert(IsValid() && "Not valid storage");
    return initial_pos_;
  }
  const triangles::AxesArray<T>& GetAxes() const {
    assert(IsValid() && "Not valid storage");
    return axes_;
  }

  const std::vector<T>& GetSpeeds() const {
    assert(IsValid() && "Not valid storage");
    return speeds_;
  }

  const shapes::ShapesNumber& GetShapesNumber() const {
    assert(IsValid() && "Not valid storage");
    return shapes_number_;
  }

  const std::vector<std::atomic_bool>& GetPredicates() const {
    assert(IsValid() && "Not valid storage");
    return updater_ctx_.predicates;
  }

  const std::vector<std::pair<size_t, size_t>>& GetPotentialPairs() const {
    assert(IsValid() && "Not valid storage");
    return updater_ctx_.potential_pairs;
  }

 private:
  triangles::TrianglesArray<T> initial_pos_;
  triangles::AxesArray<T> axes_;
  std::vector<T> speeds_;
  shapes::ShapesNumber shapes_number_;
  UpdaterContext updater_ctx_;
};

template <std::floating_point T>
class StorageUpdater {
 public:
  virtual void ChangeStorage(
      TrianglesStorage<T>& storage,
      TrianglesStorage<T>::UpdaterContext* ctx) = 0;

  virtual ~StorageUpdater() = default;
};

}  // namespace triangles

#endif  // TRIANGLES_STORAGE_HPP_
