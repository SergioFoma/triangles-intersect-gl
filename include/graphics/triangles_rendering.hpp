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

const int kOneTexture = 1;

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

// ==================== Configs For User ==========================

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
  ShaderConfig shader_con;
};

struct RenderConfig {
  CameraConfig camera_con;
  LightConfig light_con;
  ShaderConfig shader_con;
};

// =============================================================

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
  GeometryBuffer geom_buff;
  Shader shader;
  glm::mat4 model;
  glm::mat3 normal_mat;
};

struct SkyboxData {
  SkyboxConfig config;
  RenderObject obj;
  unsigned int texture_id = 0;

struct SkyboxData {
  SkyboxConfig config;
  RenderObject obj;
  unsigned int texture_id = 0;

  SkyboxData(SkyboxConfig config, RenderObject obj, unsigned int id)
    : config(std::move(config)), obj(std::move(obj)), texture_id(id) {}

  SkyboxData(const SkyboxData& other) = delete;
  SkyboxData& operator=(const SkyboxData& other) = delete;

  SkyboxData(SkyboxData&& other)
    : config(std::move(other.config)),
      obj(std::move(other.obj)),
      texture_id(other.texture_id) {

    other.texture_id = 0;
  }

  SkyboxData& operator=(SkyboxData&& other) {
    if (this == &other) {
      return *this;
    }
    glDeleteTextures(kOneTexture, &texture_id);

    config = std::move(other.config);
    obj = std::move(other.obj);
    texture_id = other.texture_id;
    other.texture_id = 0;

    return *this;
  };

  ~SkyboxData() {
    glDeleteTextures(kOneTexture, &texture_id);
  }
};

render::ErrorType RenderTriangles(GLFWwindow* win,
                                  size_t triangles_number,
                                  const std::vector<float>& triangles,
                                  const render::RenderConfig& render_con,
                                  const render::SkyboxConfig& skybox_con);
}  // namespace render

void ProcessInput(GLFWwindow* win, render::Camera& camera);

render::SkyboxData CallSkyboxCreating(const render::SkyboxConfig& skybox_con);

render::RenderObject CreateTriangleObj(const std::vector<float>& raw_data,
                                       const render::ShaderConfig& shader_con, 
                                       size_t triangles_number);

render::SkyboxData CreateSkyboxObj(const std::vector<float>& raw_data,
                                     const render::SkyboxConfig& skybox_con);

#endif
