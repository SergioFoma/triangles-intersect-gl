#ifndef UPDATER_HPP_
#define UPDATER_HPP_

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#include <GLFW/glfw3.h>

#include <type_traits>
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"

#include "triangles/triangle.hpp"
#include "triangles/triangles_storage.hpp"

namespace updater {

template <typename T>
requires std::is_floating_point_v<T>
class Updater {
 public:
  explicit Updater(double current_time): last_frame_{current_time} {};

  void Update(storage::TrianglesStorage<T>& tr_storage) {
    auto& triangles = tr_storage.GetTriangles();
    const auto& axes = tr_storage.GetAxes();
    const auto& speeds = tr_storage.GetSpeeds();

    size_t obj_sz = triangles.size();
    if (!(obj_sz == axes.size() && obj_sz == speeds.size())) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kUpdater,
                                   "Incorrect number of objects!",
                                   "Updater::Update"};
      throw prog_error::ProgError(std::move(inf));
    }

    double current_time = glfwGetTime();

    tbb::blocked_range<size_t> range{kRangeStart, triangles.size()};
    tbb::parallel_for(range, [&](tbb::blocked_range<size_t> r) {
      for (size_t ind = r.begin(); ind < r.end(); ++ind) {
        UpdateTr(triangles[ind], axes[ind], speeds[ind], current_time - last_frame_);
      }
    });
    last_frame_ = current_time;
  }

 private:
  double last_frame_ = 0.0;

  static constexpr size_t kRangeStart = 0;

  void UpdateTr(triangles::Triangle3D<T>& tr, const triangles::Axis<T>& axis,
                const T& speed, double delta) {

    glm::vec3 begin{axis.p_1_.x_, axis.p_1_.y_, axis.p_1_.z_};
    glm::vec3 end{axis.p_2_.x_, axis.p_2_.y_, axis.p_2_.z_};
    glm::vec3 trace = glm::normalize(end - begin);

    triangles::Point3D<T> first_p = tr.GetFirstPoint();
    triangles::Point3D<T> second_p = tr.GetSecondPoint();
    triangles::Point3D<T> third_p = tr.GetThirdPoint();

    glm::vec3 first_vec{first_p.x_, first_p.y_, first_p.z_};
    glm::vec3 second_vec{second_p.x_, second_p.y_, second_p.z_};
    glm::vec3 third_vec{third_p.x_, third_p.y_, third_p.z_};

    float angle = static_cast<float>(speed * delta);

    first_vec = glm::rotate(first_vec - begin, glm::radians(angle), trace) + begin;
    second_vec = glm::rotate(second_vec - begin, glm::radians(angle), trace) + begin;
    third_vec = glm::rotate(third_vec - begin, glm::radians(angle), trace) + begin;

    triangles::Point3D<T> new_first_p{first_vec.x, first_vec.y, first_vec.z};
    triangles::Point3D<T> new_second_p{second_vec.x, second_vec.y, second_vec.z};
    triangles::Point3D<T> new_third_p{third_vec.x, third_vec.y, third_vec.z};

    tr = triangles::Triangle3D<T>{new_first_p, new_second_p, new_third_p};
  }
};
} // namespace updater

#endif
