#ifndef ADAPTER_HPP_
#define ADAPTER_HPP_

#include <vector>

#include "triangles_rendering.hpp"
#include "triangle.hpp"
#include "box.hpp"

namespace adapter {

class Adapter {
 public:

  Adapter(GLFWwindow* window,
          const render::RenderConfig& render_con,
          const render::SkyboxConfig& skybox_con)
          : win_(window), render_con_(render_con),
            skybox_con_(skybox_con) {}

  void ConvertTriangle(const triangles::Triangle3D& triangle, bool intersect_status);

  void ConvertBoxLevels(const std::vector<std::vector<triangles::Box>>& levels,
      const std::vector<std::vector<std::pair<size_t, size_t>>>& ranges);

  void Draw();

 private:

  std::vector<float> raw_data_;
  std::vector<std::vector<float>> box_levels_;
  size_t triangles_number_ = 0;

  GLFWwindow* win_;
  render::RenderConfig render_con_;
  render::SkyboxConfig skybox_con_;

  static constexpr render::Color red_color_ = {1.0F, 0.0F, 0.0F};
  static constexpr render::Color white_color_ = {1.0F, 1.0F, 1.0F};

  void ConvertPoint(const triangles::Point3D& point);

  void AddColor(bool intersect_status);

  glm::vec3 GetNormal(const triangles::Triangle3D& triangle) const;

  void AddNormal(const glm::vec3& normal);

  void ConvertVertex(const triangles::Point3D& point, bool intersect_status,
                     const glm::vec3& normal);
};
} // namespace adapter


#endif
