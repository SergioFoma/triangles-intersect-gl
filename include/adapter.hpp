#ifndef ADAPTER_HPP_
#define ADAPTER_HPP_

#include <algorithm>
#include <type_traits>
#include <vector>
#include <memory>
#include <utility>

#include "basics.hpp"
#include "prog_error.hpp"
#include "triangles_rendering.hpp"
#include "triangle.hpp"

namespace adapter {

template <typename T = double>
requires std::is_floating_point_v<T>
class Adapter {
 public:

  Adapter(utility::Window window,
          const render::RenderConfig& render_con,
          const render::SkyboxConfig& skybox_con)
          : win_(std::move(window)), render_con_(render_con),
            skybox_con_(skybox_con) {}

  void ConvertShape(const triangles::Triangle3D<T>& triangle, bool intersect_status) {

    glm::vec3 normal = GetNormal(triangle);

    const typename triangles::Triangle3D<T>::TriangleType& tr_type = triangle.GetTriangleType();

    if (tr_type == triangles::Triangle3D<T>::TriangleType::kDefault) {
      ConvertTriangle(triangle, intersect_status, normal);
    } else if (tr_type == triangles::Triangle3D<T>::TriangleType::kLine) {
      ConvertLine(triangle, intersect_status, normal);
    } else {
      ConvertPoint(triangle, intersect_status, normal);
    }
  }

  void Draw() {
    render::ShapesNumber shapes_number = {
        triangles_number_ + points_number_ + lines_number_,
        triangles_number_, points_number_, lines_number_
    };

    render::ErrorType error_code  = render::RenderTriangles(std::move(win_),
                                    shapes_number, raw_data_,
                                    render_con_, skybox_con_);
    if (error_code != render::ErrorType::kCorrect) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kAdapter,
                                     "Error of rendering triangles!",
                                     "Adapter::ConvertTriangle"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  static void SortingByType(triangles::TriangleArray<T>& triangles) {
    const auto comparator = [](const triangles::Triangle3D<T>& tr1,
                                 const triangles::Triangle3D<T>& tr2) {
      return static_cast<int>(tr1.GetTriangleType())
      < static_cast<int>(tr2.GetTriangleType());
    };


    std::sort(triangles.begin(), triangles.end(), comparator);
  }

 private:

  std::vector<float> raw_data_;
  size_t triangles_number_ = 0;
  size_t points_number_ = 0;
  size_t lines_number_ = 0;

  utility::Window win_;
  render::RenderConfig render_con_;
  render::SkyboxConfig skybox_con_;

  static constexpr render::Color red_color_ = {1.0F, 0.0F, 0.0F};
  static constexpr render::Color white_color_ = {1.0F, 1.0F, 1.0F};

  void ConvertTriangle(const triangles::Triangle3D<T>& triangle,
                       bool intersect_status, const glm::vec3& normal) {

    ConvertVertex(triangle.GetFirstPoint(), intersect_status, normal);
    ConvertVertex(triangle.GetSecondPoint(), intersect_status, normal);
    ConvertVertex(triangle.GetThirdPoint(), intersect_status, normal);

    ++triangles_number_;
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

    ++lines_number_;
  }

  void ConvertPoint(const triangles::Triangle3D<T>& triangle,
                       bool intersect_status, const glm::vec3& normal) {
    ConvertVertex(triangle.GetFirstPoint(), intersect_status, normal);

    ++points_number_;
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
