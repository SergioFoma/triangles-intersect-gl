#include "adapter.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "inter_analyzer.hpp"
#include "triangle.hpp"

glm::vec3 adapter::Adapter::GetNormal(const triangles::Triangle3D& triangle) const {

  const triangles::Surface& surface = triangle.GetSurface();
  const triangles::Point3D& norm = surface.norm_;

  assert(surface.IsValid());

  glm::vec3 normal = {norm.x_, norm.y_, norm.z_};

 return normal;
}

void adapter::Adapter::ConvertPoint(const triangles::Point3D& point) {
  raw_data_.push_back(static_cast<float>(point.x_));
  raw_data_.push_back(static_cast<float>(point.y_));
  raw_data_.push_back(static_cast<float>(point.z_));
}

void adapter::Adapter::AddColor(bool intersect_status) {
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

void adapter::Adapter::AddNormal(const glm::vec3& normal) {
  raw_data_.push_back(normal.x);
  raw_data_.push_back(normal.y);
  raw_data_.push_back(normal.z);
}

void adapter::Adapter::ConvertVertex(const triangles::Point3D& point,
                            bool intersect_status,
                            const glm::vec3& normal) {
  ConvertPoint(point);
  AddColor(intersect_status);
  AddNormal(normal);
}

void adapter::Adapter::ConvertTriangle(const triangles::Triangle3D& triangle,
                                bool intersect_status) {

  glm::vec3 normal = GetNormal(triangle);

  ConvertVertex(triangle.GetFirstPoint(), intersect_status, normal);
  ConvertVertex(triangle.GetSecondPoint(), intersect_status, normal);
  ConvertVertex(triangle.GetThirdPoint(), intersect_status, normal);

  ++triangles_number_;
}

void adapter::Adapter::ConvertBoxLevels(const std::vector<std::vector<triangles::Box>>& levels,
      const std::vector<std::vector<std::pair<size_t, size_t>>>& ranges) {
  box_levels_.clear();
  constexpr render::Color colors[] = {
    {0.1F, 1.0F, 0.35F}, {1.0F, 0.65F, 0.05F}, {0.1F, 0.8F, 1.0F},
    {1.0F, 0.2F, 0.8F}, {0.8F, 0.7F, 1.0F}, {1.0F, 1.0F, 0.2F}};
  constexpr int edges[][2] = {{0,1},{2,3},{4,5},{6,7},
                              {0,2},{1,3},{4,6},{5,7},
                              {0,4},{1,5},{2,6},{3,7}};
  for (size_t level = 0; level < levels.size(); ++level) {
    auto& data = box_levels_.emplace_back();
    const auto color = colors[level % 6];
    for (size_t index = 0; index < levels[level].size(); ++index) {
      const auto& box = levels[level][index];
      const auto [first, end] = ranges[level][index];
      const float rank = levels[0].size() > 1
        ? static_cast<float>(first + end - 1) / (2.0F * (levels[0].size() - 1)) : 0.5F;
      // Blue -> cyan -> yellow -> red along the actual sorted leaf ranks.
      const glm::vec3 morton = glm::clamp(glm::vec3(1.5F) -
        glm::abs(4.0F * rank - glm::vec3(3.0F, 2.0F, 1.0F)), 0.0F, 1.0F);
      for (const auto& edge : edges) {
        for (int corner : edge) {
          data.insert(data.end(), {
            static_cast<float>((corner & 1) ? box.max_.x_ : box.min_.x_),
            static_cast<float>((corner & 2) ? box.max_.y_ : box.min_.y_),
            static_cast<float>((corner & 4) ? box.max_.z_ : box.min_.z_),
            color.r, color.g, color.b, morton.r, morton.g, morton.b});
        }
      }
    }
  }
}

void adapter::Adapter::Draw() {
  render::ErrorType error_code  = render::RenderTriangles(win_,
                                  triangles_number_, raw_data_,
                                  render_con_, skybox_con_, box_levels_);
  if (error_code != render::ErrorType::kCorrect) {
    throw std::runtime_error("Adapter::Draw: error of Rendering Triangles!");
  }
}
