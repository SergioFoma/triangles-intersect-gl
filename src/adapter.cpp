#include <algorithm>

#include "adapter.hpp"
#include "basics.hpp"
#include "inter_analyzer.hpp"
#include "triangle.hpp"
#include "prog_error.hpp"

glm::vec3 adapter::Adapter::GetNormal(const triangles::Triangle3D& triangle) const {

  triangles::Surface surface = triangle.GetSurface();
  triangles::Point3D& norm = surface.norm_;

  if (triangle.GetTriangleType() != triangles::Triangle3D::TriangleType::kDefault) {
    norm = {0.0F, 0.0F, 0.0F};
  }

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

void adapter::Adapter::ConvertTriangle(const triangles::Triangle3D &triangle,
                                       bool intersect_status,
                                       const glm::vec3 &normal) {

  ConvertVertex(triangle.GetFirstPoint(), intersect_status, normal);
  ConvertVertex(triangle.GetSecondPoint(), intersect_status, normal);
  ConvertVertex(triangle.GetThirdPoint(), intersect_status, normal);

  ++triangles_number_;
}

void adapter::Adapter::ConvertLine(const triangles::Triangle3D& triangle,
                                   bool intersect_status,
                                   const glm::vec3& normal) {

  std::pair<triangles::Point3D, triangles::Point3D> ends;
  const triangles::Point3D& p_1 = triangle.GetFirstPoint();
  const triangles::Point3D& p_2 = triangle.GetSecondPoint();
  const triangles::Point3D& p_3 = triangle.GetThirdPoint();
  double dist_1 = triangles::Point3D::Distance(p_2, p_1);
  double dist_2 = triangles::Point3D::Distance(p_3, p_2);
  double dist_3 = triangles::Point3D::Distance(p_1, p_3);

  if (dist_1 > dist_2 && dist_1 > dist_3)      ends = {p_1, p_2};
  else if (dist_2 > dist_1 && dist_2 > dist_3) ends = {p_3, p_2};
  else ends = {p_1, p_3};

  ConvertVertex(ends.first, intersect_status, normal);
  ConvertVertex(ends.second, intersect_status, normal);

  ++lines_number_;
}

void adapter::Adapter::ConvertPoint(const triangles::Triangle3D& triangle,
                                    bool intersect_status,
                                    const glm::vec3& normal) {
  ConvertVertex(triangle.GetFirstPoint(), intersect_status, normal);

  ++points_number_;
}

void adapter::Adapter::ConvertShape(const triangles::Triangle3D& triangle,
                                       bool intersect_status) {

  glm::vec3 normal = GetNormal(triangle);

  const triangles::Triangle3D::TriangleType& tr_type = triangle.GetTriangleType();

  if (tr_type == triangles::Triangle3D::TriangleType::kDefault) {
    ConvertTriangle(triangle, intersect_status, normal);
  } else if (tr_type == triangles::Triangle3D::TriangleType::kLine) {
    ConvertLine(triangle, intersect_status, normal);
  } else {
    ConvertPoint(triangle, intersect_status, normal);
  }
}

void adapter::Adapter::Draw() {
  render::ShapesNumber shapes_number = {
      triangles_number_ + points_number_ + lines_number_,
      triangles_number_, points_number_, lines_number_
  };

  render::ErrorType error_code  = render::RenderTriangles(win_,
                                  shapes_number, raw_data_,
                                  render_con_, skybox_con_);
  if (error_code != render::ErrorType::kCorrect) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kAdapter,
                                   "Error of rendering triangles!",
                                   "Adapter::ConvertTriangle"};
    throw prog_error::ProgError(std::move(inf));
  }
}

void adapter::Adapter::SortingByType(triangles::TriangleArr& triangles) {
  const auto comparator = [](const triangles::Triangle3D& tr1,
                               const triangles::Triangle3D& tr2) {
    return static_cast<int>(tr1.GetTriangleType())
    < static_cast<int>(tr2.GetTriangleType());
  };


  std::sort(triangles.begin(), triangles.end(), comparator);
}
