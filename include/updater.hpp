#ifndef UPDATER_HPP_
#define UPDATER_HPP_

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#include <numbers>
#include <type_traits>
#include "basics.hpp"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"

#include "triangles/triangle.hpp"
#include "triangles/triangles_storage.hpp"

namespace updater {

template <typename T>
requires std::is_floating_point_v<T> class Updater {
 public:
  explicit Updater(storage::TrianglesStorage<T>& tr_storage,
                   double current_time)
    : initial_pos_(tr_storage.GetShapesNumber().shapes_number),
      init_frame_{current_time} {

    const triangles::TrianglesArray<T>& triangles = tr_storage.GetTriangles();

    tbb::blocked_range<size_t> range{kRangeStart, initial_pos_.size()};
    tbb::parallel_for(range, [&](tbb::blocked_range<size_t> r) {
      for (size_t ind = r.begin(); ind < r.end(); ++ind) {
        initial_pos_[ind] = triangles[ind];
      }
    });
  };

  void Update(storage::TrianglesStorage<T>& tr_storage) {
    auto& triangles = tr_storage.GetTriangles();
    const auto& axes = tr_storage.GetAxes();
    const auto& speeds = tr_storage.GetSpeeds();
    auto& predicates = tr_storage.GetPredicates();
    if (predicates.size() != triangles.size()) {
      std::vector<std::atomic_bool> resized(triangles.size());
      predicates.swap(resized);
    }

    size_t obj_sz = triangles.size();
    if (!(obj_sz == axes.size() && obj_sz == speeds.size())) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kUpdater,
                                   "Incorrect number of objects!",
                                   "Updater::Update"};
      throw prog_error::ProgError(std::move(inf));
    }

    double delta = glfwGetTime() - init_frame_;

    tbb::blocked_range<size_t> range{kRangeStart, triangles.size()};
    tbb::parallel_for(range, [&](tbb::blocked_range<size_t> r) {
      for (size_t ind = r.begin(); ind < r.end(); ++ind) {
        UpdateTr(triangles[ind], initial_pos_[ind],
                 axes[ind], speeds[ind],
                 delta, predicates[ind]);
      }
    });
  }

 private:
  triangles::TrianglesArray<T> initial_pos_;
  double init_frame_ = 0.0;

  static constexpr size_t kRangeStart = 0;

  void UpdateTr(triangles::Triangle3D<T>& tr, const triangles::Triangle3D<T>& init_pos,
                const triangles::Line<T>& axis, const T& speed,
                T delta, std::atomic_bool& predicate) {

    const auto convert_to_radians = [](T grades) {
      const T half_angle = T{180};
      const T coeff = std::numbers::pi_v<T> / half_angle;
      return coeff * grades;
    };
    const T angle = convert_to_radians(speed * delta);
    const triangles::Point3D<T> new_first_p =
        axis.RotatePoint(init_pos.GetFirstPoint(), angle);
    const triangles::Point3D<T> new_second_p =
        axis.RotatePoint(init_pos.GetSecondPoint(), angle);
    const triangles::Point3D<T> new_third_p =
        axis.RotatePoint(init_pos.GetThirdPoint(), angle);
    predicate.store(false, std::memory_order_relaxed);

    tr = triangles::Triangle3D<T>{new_first_p, new_second_p, new_third_p};
  }
};
}  // namespace updater

#endif
