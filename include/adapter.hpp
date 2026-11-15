#ifndef ADAPTER_HPP_
#define ADAPTER_HPP_

#include <tbb/parallel_sort.h>
#include <algorithm>
#include <type_traits>
#include <vector>
#include <memory>
#include <utility>

#include "graphics/graphics_container.hpp"
#include "graphics/geometry_buffer.hpp"
#include "graphics/utility.hpp"
#include "graphics/triangles_rendering.hpp"

#include "triangles/triangles_storage.hpp"
#include "triangles/inter_analyzer.hpp"

#include "prog_error.hpp"
#include "configs.hpp"

namespace adapter {

template <typename T = double>
requires std::is_floating_point_v<T>
class Adapter {
 public:

  explicit Adapter(storage::TrianglesStorage<T>& storage)
    : raw_data_(storage.GetShapesNumber().triangles_number * kTrianglePoints * kPointDim +
                storage.GetShapesNumber().lines_number * kLinePoints * kPointDim +
                storage.GetShapesNumber().points_number * kPointDim) {

    const auto comparator = [](const triangles::Triangle3D<T>& tr1,
                                 const triangles::Triangle3D<T>& tr2) {
      return static_cast<int>(tr1.GetTriangleType())
      < static_cast<int>(tr2.GetTriangleType());
    };

    triangles::TrianglesArray<T>& triangles = storage.GetTriangles();
    tbb::parallel_sort(triangles.begin(), triangles.end(), comparator);
  }

  void ConvertShapes(triangles::TrianglesArray<T>& triangles,
                     const triangles::InterAnalyzer<T>& analyzer,
                     const shapes::ShapesNumber& shapes_num) {

    tbb::blocked_range<size_t> range{0, shapes_num.shapes_number};
    tbb::parallel_for(range, [&](tbb::blocked_range<size_t> r) {
      for (size_t ind = r.begin(); ind < r.end(); ++ind) {
        ConvertShape(triangles[ind], shapes_num, analyzer.DoesIntersect(ind), ind);
      }
    });
  }
  void ConvertShape(const triangles::Triangle3D<T>& triangle,
                    const shapes::ShapesNumber& shapes_num,
                    bool intersect_status, size_t ind) {
    size_t tr_beg = 0, tr_end = shapes_num.triangles_number;
    size_t line_beg = tr_end, line_end = line_beg + shapes_num.lines_number;
    size_t raw_beg = 0;

    glm::vec3 normal = GetNormal(triangle);

    if (tr_beg <= ind && ind < tr_end) {
      raw_beg = ind * kTrianglePoints * kPointDim;
      ConvertTriangle(triangle, intersect_status, normal, raw_beg);
    } else if (line_beg <= ind && ind < line_end) {
      raw_beg = shapes_num.triangles_number * kTrianglePoints * kPointDim +
                (ind - line_beg) * kLinePoints * kPointDim;
      ConvertLine(triangle, intersect_status, normal, raw_beg);
    } else {
      raw_beg = shapes_num.triangles_number * kTrianglePoints * kPointDim +
                shapes_num.lines_number * kLinePoints * kPointDim +
                (ind - line_end) * kPointDim;
      ConvertPoint(triangle, intersect_status, normal, raw_beg);
    }
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
 private:

  std::vector<float> raw_data_;

  static constexpr graphics::Color red_color_ = {1.0F, 0.0F, 0.0F};
  static constexpr graphics::Color white_color_ = {1.0F, 1.0F, 1.0F};
  static constexpr size_t kPointDim = 9;   // (x, y, z, r, g, b, n_x, n_y, n_z)
  static constexpr size_t kTrianglePoints = 3; // triangle has 3 points
  static constexpr size_t kLinePoints = 2;     // line has 2 points

  void ConvertTriangle(const triangles::Triangle3D<T>& triangle,
                       bool intersect_status, const glm::vec3& normal,
                       size_t raw_ind) {

    ConvertVertex(triangle.GetFirstPoint(), intersect_status, normal, raw_ind);
    ConvertVertex(triangle.GetSecondPoint(), intersect_status, normal, raw_ind);
    ConvertVertex(triangle.GetThirdPoint(), intersect_status, normal, raw_ind);
  }

  void ConvertLine(const triangles::Triangle3D<T>& triangle,
                   bool intersect_status, const glm::vec3& normal,
                   size_t raw_ind) {

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

    ConvertVertex(ends.first, intersect_status, normal, raw_ind);
    ConvertVertex(ends.second, intersect_status, normal, raw_ind);
  }

  void ConvertPoint(const triangles::Triangle3D<T>& triangle,
                    bool intersect_status, const glm::vec3& normal,
                    size_t raw_ind) {
    ConvertVertex(triangle.GetFirstPoint(), intersect_status, normal, raw_ind);
  }

  void ConvertPoint(const triangles::Point3D<T>& point, size_t& raw_ind) {
    raw_data_[raw_ind++] = static_cast<float>(point.x_);
    raw_data_[raw_ind++] = static_cast<float>(point.y_);
    raw_data_[raw_ind++] = static_cast<float>(point.z_);
  }

  void AddColor(bool intersect_status, size_t& raw_ind) {
    if (intersect_status) {
      raw_data_[raw_ind++] = red_color_.r;
      raw_data_[raw_ind++] = red_color_.g;
      raw_data_[raw_ind++] = red_color_.b;

      return ;
    }

    raw_data_[raw_ind++] = white_color_.r;
    raw_data_[raw_ind++] = white_color_.g;
    raw_data_[raw_ind++] = white_color_.b;
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

  void AddNormal(const glm::vec3& normal, size_t& raw_ind) {
    raw_data_[raw_ind++] = normal.x;
    raw_data_[raw_ind++]= normal.y;
    raw_data_[raw_ind++] = normal.z;
  }

  void ConvertVertex(const triangles::Point3D<T>& point, bool intersect_status,
                     const glm::vec3& normal, size_t& raw_ind) {
    ConvertPoint(point, raw_ind);
    AddColor(intersect_status, raw_ind);
    AddNormal(normal, raw_ind);
  }
};
} // namespace adapter


#endif
