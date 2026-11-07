#ifndef ADAPTER_HPP_
#define ADAPTER_HPP_

#include <algorithm>
#include <type_traits>
#include <vector>
#include <memory>
#include <utility>

#include "graphics/graphics_container.hpp"
#include "graphics/geometry_buffer.hpp"
#include "graphics/utility.hpp"
#include "graphics/triangles_rendering.hpp"

#include "triangles/triangle.hpp"

#include "prog_error.hpp"
#include "configs.hpp"

namespace adapter {

template <typename T = double>
requires std::is_floating_point_v<T>
class Adapter {
 public:

  Adapter() = default;

  void ConvertShape(const triangles::Triangle3D<T>& triangle, bool intersect_status) {

    glm::vec3 normal = GetNormal(triangle);

    const typename triangles::Triangle3D<T>::TriangleType& tr_type = triangle.GetTriangleType();

    if (tr_type == triangles::TriangleType<T>::kDefault) {
      ConvertTriangle(triangle, intersect_status, normal);
    } else if (tr_type == triangles::TriangleType<T>::kLine) {
      ConvertLine(triangle, intersect_status, normal);
    } else {
      ConvertPoint(triangle, intersect_status, normal);
    }
  }

  void Clear() {
    raw_data_.clear();
  }

  void Draw(utility::Window& win,
            video_buff::BufferManager& buff_manager,
            graphics::GraphicsContainer& graphics_container,
            const config::UserConfigs& configs) {

    render::ErrorType error_code  = render::RenderTriangles(win, buff_manager,
                                                            graphics_container,
                                                            configs,
                                                            raw_data_);
    if (error_code != render::ErrorType::kCorrect) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kAdapter,
                                     "Error of rendering triangles!",
                                     "Adapter::ConvertTriangle"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  static void SortingByType(triangles::TrianglesArray<T>& triangles) {
    const auto comparator = [](const triangles::Triangle3D<T>& tr1,
                                 const triangles::Triangle3D<T>& tr2) {
      return static_cast<int>(tr1.GetTriangleType())
      < static_cast<int>(tr2.GetTriangleType());
    };


    std::sort(triangles.begin(), triangles.end(), comparator);
  }

 private:

  std::vector<float> raw_data_;

  static constexpr graphics::Color red_color_ = {1.0F, 0.0F, 0.0F};
  static constexpr graphics::Color white_color_ = {1.0F, 1.0F, 1.0F};

  void ConvertTriangle(const triangles::Triangle3D<T>& triangle,
                       bool intersect_status, const glm::vec3& normal) {

    ConvertVertex(triangle.GetFirstPoint(), intersect_status, normal);
    ConvertVertex(triangle.GetSecondPoint(), intersect_status, normal);
    ConvertVertex(triangle.GetThirdPoint(), intersect_status, normal);
  }

  void ConvertLine(const triangles::Triangle3D<T>& triangle,
                       bool intersect_status, const glm::vec3& normal) {

    std::pair<triangles::Point3D<T>, triangles::Point3D<T>> ends;
    const triangles::Point3D<T>& p_1 = triangle.GetFirstPoint();
    const triangles::Point3D<T>& p_2 = triangle.GetSecondPoint();
    const triangles::Point3D<T>& p_3 = triangle.GetThirdPoint();
    T dist_1 = triangles::Point3D<T>::Distance(p_2, p_1);
    T dist_2 = triangles::Point3D<T>::Distance(p_3, p_2);
    T dist_3 = triangles::Point3D<T>::Distance(p_1, p_3);

    if (dist_1 > dist_2 && dist_1 > dist_3)      ends = {p_1, p_2};
    else if (dist_2 > dist_1 && dist_2 > dist_3) ends = {p_3, p_2};
    else ends = {p_1, p_3};

    ConvertVertex(ends.first, intersect_status, normal);
    ConvertVertex(ends.second, intersect_status, normal);
  }

  void ConvertPoint(const triangles::Triangle3D<T>& triangle,
                       bool intersect_status, const glm::vec3& normal) {
    ConvertVertex(triangle.GetFirstPoint(), intersect_status, normal);
  }

  void ConvertPoint(const triangles::Point3D<T>& point) {
    raw_data_.push_back(static_cast<float>(point.x_));
    raw_data_.push_back(static_cast<float>(point.y_));
    raw_data_.push_back(static_cast<float>(point.z_));
  }

  void AddColor(bool intersect_status) {
    if (intersect_status) {
      raw_data_.push_back(red_color_.r);
      raw_data_.push_back(red_color_.g);
      raw_data_.push_back(red_color_.b);

      return ;
    }

    raw_data_.push_back(white_color_.r);
    raw_data_.push_back(white_color_.g);
    raw_data_.push_back(white_color_.b);
  }

  glm::vec3 GetNormal(const triangles::Triangle3D<T>& triangle) const {

    triangles::Surface<T> surface = triangle.GetSurface();
    triangles::Point3D<T>& norm = surface.norm_;

    if (triangle.GetTriangleType() != triangles::Triangle3D<T>::TriangleType::kDefault) {
      norm = {0.0F, 0.0F, 0.0F};
    }

    glm::vec3 normal = {norm.x_, norm.y_, norm.z_};

   return normal;
  }

  void AddNormal(const glm::vec3& normal) {
    raw_data_.push_back(normal.x);
    raw_data_.push_back(normal.y);
    raw_data_.push_back(normal.z);
  }

  void ConvertVertex(const triangles::Point3D<T>& point, bool intersect_status,
                     const glm::vec3& normal) {
    ConvertPoint(point);
    AddColor(intersect_status);
    AddNormal(normal);
  }
};
} // namespace adapter


#endif
