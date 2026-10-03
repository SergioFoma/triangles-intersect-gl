
#include "adapter.hpp"
#include "inter_analyzer.hpp"
#include "triangle.hpp"

glm::vec3 adapter::Adapter::GetNormal(const triangles::Triangle3D& triangle) const {
  const triangles::Point3D& p_1 = triangle.GetFirstPoint();
  const triangles::Point3D& p_2 = triangle.GetSecondPoint();
  const triangles::Point3D& p_3 = triangle.GetThirdPoint();

  glm::vec3 v_12 = glm::vec3(p_1.x_ - p_2.x_, p_1.y_ - p_2.y_,
                                    p_1.z_ - p_2.z_);
  glm::vec3 v_13 = glm::vec3(p_1.x_ - p_3.x_, p_1.y_ - p_3.y_,
                                    p_1.z_ - p_3.z_);
  glm::vec3 normal = glm::normalize(glm::cross(v_12, v_13));

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

void adapter::Adapter::Draw() {
  render::RenderTriangles(win_, triangles_number_, raw_data_,
                          render_con_, skybox_con_);
}
