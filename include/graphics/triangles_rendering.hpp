#ifndef TRIANGLES_RENDERING_HPP_
#define TRIANGLES_RENDERING_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <memory>
#include <cstdint>

#include "geometry_buffer.hpp"
#include "shader.hpp"
#include "utility.hpp"

namespace render {

constexpr unsigned int kOneTexture = 1;

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

struct ShapesNumber {
  size_t shapes_number;
  size_t triangles_number;
  size_t points_number;
  size_t lines_number;
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
  float near_plane = 0.0F;
  float far_plane = 0.0F;
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
  ShaderConfig tr_shader_con;
  ShaderConfig point_shader_con;
  ShaderConfig line_shader_con;
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
  float fovy = 0.0F;
  float aspect = 0.0F;
  float near_plane = 0.0F;
  float far_plane = 0.0F;

  explicit Camera(const CameraConfig& camera)
      : pos(camera.pos), front(camera.front), up(camera.up),
        speed(camera.speed), sensitivity(camera.mouse_sensitivity),
        fovy(camera.fovy), aspect(camera.aspect),
        near_plane(camera.near_plane), far_plane(camera.far_plane) {
    last_frame = glfwGetTime();

    projection = glm::perspective(glm::radians(fovy), aspect, near_plane, far_plane);
  }

  void UpdateCameraMatrix() {
    view = glm::lookAt(pos, pos + front, up);
  }

  void SetPerspectiveMatrix(float new_aspect) {
    projection = glm::perspective(glm::radians(fovy), new_aspect, near_plane, far_plane);
  }
};

struct RenderObject {
  std::shared_ptr<GeometryBuffer> geom_buff;
  Shader shader;
  glm::mat4 model;
  glm::mat3 normal_mat;
  size_t start_pos = 0;
  size_t count = 0;
};

struct ShapesObject {
  RenderObject tr_obj;
  RenderObject point_obj;
  RenderObject line_obj;
};

struct TextureIdDeleter {
  int one_texture = 1;

  void operator()(unsigned int* texture_id) const {
    if (!texture_id) {
      return ;
    }

    std::uintptr_t uint_id = reinterpret_cast<std::uintptr_t>(texture_id);
    unsigned int unint_id = static_cast<unsigned int>(uint_id);
    glDeleteTextures(one_texture, &unint_id);
  }
};

using TextureId = std::unique_ptr<unsigned int, TextureIdDeleter>;

struct SkyboxData {
  SkyboxConfig config;
  RenderObject obj;
  TextureId texture_id;

  SkyboxData(SkyboxConfig config, RenderObject obj, TextureId id)
    : config(std::move(config)), obj(std::move(obj)),
      texture_id(std::move(id)) {}

  SkyboxData(const SkyboxData& other) = delete;

  SkyboxData(SkyboxData&& other) = default;

  unsigned int GetTextureId() const {
    std::uintptr_t uint_id = reinterpret_cast<std::uintptr_t>(texture_id.get());
    unsigned int unint_id = static_cast<unsigned int>(uint_id);

    return unint_id;
  }

  SkyboxData& operator=(const SkyboxData& other) = delete;

  SkyboxData& operator=(SkyboxData&& other) = default;

  ~SkyboxData() = default;
};

render::ErrorType RenderTriangles(utility::Window win,
                                  const ShapesNumber& shapes_number,
                                  const std::vector<float>& triangles,
                                  const render::RenderConfig& render_con,
                                  const render::SkyboxConfig& skybox_con);
}  // namespace render

#endif
