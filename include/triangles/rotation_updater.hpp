#ifndef ROTATION_UPDATER_HPP_
#define ROTATION_UPDATER_HPP_

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#include <cassert>
#include <concepts>
#include <numbers>
#include "basics.hpp"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"

#include "triangles/triangle.hpp"
#include "triangles_storage.hpp"


namespace triangles {

template <std::floating_point T>
class RotationUpdater : public StorageUpdater<T> {
 public:
  explicit RotationUpdater(double current_time)
      : init_frame_{current_time} {}

  void ChangeStorage(
      TrianglesStorage<T>& tr_storage,
      TrianglesStorage<T>::UpdaterContext* ctx) override {
    assert(ctx);
    auto& triangles = ctx->current_pos;

    const auto& initial_pos = tr_storage.GetInitialPos();
    const auto& axes = tr_storage.GetAxes();
    const auto& speeds = tr_storage.GetSpeeds();

    assert(triangles.size() == initial_pos.size() &&
           triangles.size() == axes.size() &&
           triangles.size() == speeds.size());

    double delta = glfwGetTime() - init_frame_;

    tbb::blocked_range<size_t> range{kRangeStart, triangles.size()};
    tbb::parallel_for(range, [&](tbb::blocked_range<size_t> r) {
      for (size_t ind = r.begin(); ind < r.end(); ++ind) {
        UpdateTr(triangles[ind], initial_pos[ind], axes[ind], speeds[ind],
                 delta);
      }
    });
  }

 private:
  double init_frame_ = 0.0;
  static constexpr size_t kRangeStart = 0;

  void UpdateTr(triangles::Triangle3D<T>& tr,
                const triangles::Triangle3D<T>& init_pos,
                const triangles::Line<T>& axis, const T& speed, T delta) {

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

    tr = triangles::Triangle3D<T>{new_first_p, new_second_p, new_third_p};
  }
};
}  // namespace triangles

#endif
