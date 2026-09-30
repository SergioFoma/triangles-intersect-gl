#ifndef TRIANGLES_RENDERING_HPP_
#define TRIANGLES_RENDERING_HPP_

#include "geometry_buffer.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <memory>

namespace render {

enum class ErrorType { kCorrect, kError };

struct Point {
  float x = 0.0F;
  float y = 0.0F;
  float z = 0.0F;
};

struct Color {
  float r = 0.0F;
  float g = 0.0F;
  float b = 0.0F;
};

struct Triangle {
  Point p_1;
  Point p_2;
  Point p_3;
  Color color;
};

// =============== Configs For User =================

struct LightConfig {
  glm::vec3 color = glm::vec3(1.0F, 1.0F, 1.0F);
  float cut_off = 12.5F;
  float outer_cut_off = 17.5F;
  float constant = 1.0F;
  float linear = 0.09F;
  float quadratic = 0.032F;
};

struct CameraConfig {
  glm::vec3 pos;
  glm::vec3 front;
  glm::vec3 up;
  float speed = 0.0F;
  float aspect = 0.0F;
  float fovy = 0.0F;
  float near = 0.0F;
  float far = 0.0F;
  float mouse_sensitivity = 0.0F;
};

struct ShaderConfig {
  std::string vert_path =
    "./shaders/triangle.vert";    // triangle vertex shader
  std::string frag_path =
    "./shaders/triangle.frag";    // triangle fragment shader
  int position_loc = 0;           // location in shaders
  int color_loc = 1;              // location in shaders
  int normal_loc = 2;             // location in shaders
};

struct SkyboxConfig {
  std::vector<std::string> texture_sides;
  unsigned int sides_number = 0;
  unsigned int triangles_number = 0;
  unsigned int texture_id = 0;

  ShaderConfig shader_con;
};

struct RenderConfig {
  CameraConfig camera_con;
  LightConfig light_con;
  ShaderConfig shader_con;
};

// =================================================

struct Camera {
  glm::mat4 view = glm::mat4(1.0F);
  glm::mat4 projection = glm::mat4(1.0F);
  glm::vec3 pos;
  glm::vec3 front;
  glm::vec3 up;
  float speed = 0.05F;
  float delta_time = 0.0F;
  float last_frame = 0.0F;
  float yaw = -90.0F;
  float pitch = 0.0F;
  float sensitivity = 0.0F;

  explicit Camera(const CameraConfig& camera)
      : pos(camera.pos), front(camera.front), up(camera.up),
        speed(camera.speed), sensitivity(camera.mouse_sensitivity) {
    last_frame = glfwGetTime();

    projection = glm::perspective(glm::radians(camera.fovy), camera.aspect,
                                  camera.near, camera.far);
  }

  void UpdateCameraMatrix() {
    view = glm::lookAt(pos, pos + front, up);
  }
};

struct RenderObject {
  std::unique_ptr<GeometryBuffer> geom_buff;
  std::unique_ptr<Shader> shader;
  glm::mat4 model;
  glm::mat3 normal_mat;
};
}  // namespace render


render::ErrorType RenderTriangles(GLFWwindow* win,
                                  const std::vector<render::Triangle>& triangles,
                                  const render::RenderConfig& render_con,
                                  render::SkyboxConfig& skybox_con);

void InitData(std::vector<float>& data, const std::vector<render::Triangle>& triangles);

void AddPointData(std::vector<float>& data, const render::Point& point,
                  const render::Color& color);

void AddNormalData(std::vector<float>& data, const glm::vec3& normal);

void RenderCycle(GLFWwindow* win, render::RenderObject& tr_obj,
                 render::RenderObject& skybox_obj,
                 const render::RenderConfig& render_con,
                 const render::SkyboxConfig& skybox_con);

void DrawTriangles(render::RenderObject& tr_obj, const render::Camera& camera, 
                  const render::LightConfig& light_con);

void DrawSkybox(const render::RenderObject& shader_obj,
                const render::SkyboxConfig& skybox_con,
                const render::Camera& camera);

void UpdateTrPos(render::RenderObject& tr_obj, const render::Camera& camera,
                 const render::LightConfig& light_con);

void UpdateSkybox(const render::RenderObject& skybox_obj,
                  const render::Camera& camera);

void LoadCubemap(render::SkyboxConfig& skybox_con);

void ProcessInput(GLFWwindow* win, render::Camera& camera);

render::RenderObject CallSkyboxCreating(render::SkyboxConfig& skybox_con);

render::RenderObject CreateTriangleObj(const std::vector<float>& raw_data,
                                       const render::ShaderConfig& shader_con, 
                                       size_t triangles_number);

render::RenderObject CreateSkyboxObj(const std::vector<float>& raw_data,
                                     render::SkyboxConfig& skybox_con);

#endif
