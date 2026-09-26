#ifndef TRIANGLES_RENDERING_HPP_
#define TRIANGLES_RENDERING_HPP_

#include "geometry_buffer.hpp"
#include "Shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utility.hpp"

#include <string>
#include <vector>
#include <memory>

namespace render {
const int kDimension = 3;   // R^3 (x, y, z)
const int kVertexes = 3;    // Point p_1, p_2, p_3
const int kColors = 3;      // (r, g, b)
const int kNormal = 0;      // (n_x, n_y, n_z)
const float kFovy = 45.0F;  // Field of View
const float kAspect = utility::kWidth / utility::kHeight;
const float kNear = 0.1F;           // zNear
const float kFar = 100.0F;          // zFar
const float kRotateAngle = -55.0F;  // for rotate
const std::string kTrVertPath =
    "./shaders/triangle.vert";  // triangle vertex shader
const std::string kTrFragPath =
    "./shaders/triangle.frag";  // triangle fragment shader
const std::string kLgVertPath = "./shaders/light.vert";  // light vertex shader
const std::string kLgFragPath = "./shaders/light.frag";  // light fragment shader
const int kBuffCount = 1;
const int kZeroLocation = 0;   // location in shaders
const int kFirstLocation = 1;  // location in shaders
const int kOneMatrix = 1;

enum class ErrorType { kCorrect, kError };
}  // namespace render

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

struct Camera {
  glm::vec3 pos;
  glm::vec3 front;
  glm::vec3 up;
  float speed = 0.05F;
  float delta_time = 0.0F;
  float last_frame = 0.0F;
  float yaw = -90.0F;
  float pitch = 0.0F;
  glm::mat4 view = glm::mat4(1.0F);
  glm::mat4 projection = glm::mat4(1.0F);

  Camera(const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up,
         float speed)
      : pos(pos), front(front), up(up), speed(speed) {
    last_frame = glfwGetTime();

    projection = glm::perspective(glm::radians(render::kFovy), render::kAspect,
                                  render::kNear, render::kFar);
  }

  void UpdateCameraMatrix() {
    view = glm::lookAt(pos, pos + front, up);
  }
};

struct RenderObject {
  std::unique_ptr<GeometryBuffer> geom_buff;
  std::unique_ptr<Shader> shader;
  glm::mat4 model;
};

struct LightSource {
    RenderObject& light_obj;
    glm::vec3 source_color;

    LightSource(RenderObject& obj, const glm::vec3& colors)
        : light_obj(obj), source_color(colors) {}
};

render::ErrorType RenderTriangles(GLFWwindow* win,
                                  const std::vector<Triangle>& triangles);

void InitData(std::vector<float>& data, const std::vector<Triangle>& triangles);

void AddPointData(std::vector<float>& data, const Point& point,
                  const Color& color);

void RenderCycle(GLFWwindow* win, RenderObject& tr_obj, LightSource& light_source);

Camera CameraSettings();

void UpdateTrPos(RenderObject& tr_obj, const Camera& camera, const glm::vec3& lg_color);

void UpdateLgPos(LightSource& light_source, const Camera& camera);

void ProcessInput(GLFWwindow* win, Camera& camera);

RenderObject CreateTriangleObj(std::vector<float>& raw_data, size_t triangles_number);

RenderObject CreateLightObj();

//unsigned int LightSettings(unsigned vbo, unsigned int& light_vao);

#endif
