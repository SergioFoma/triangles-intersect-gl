#ifndef TRIANGLES_RENDERING_HPP_
#define TRIANGLES_RENDERING_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "geometry_buffer.hpp"
#include "shader.hpp"
#include "utility.hpp"

#include <string>
#include <vector>

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
const std::string kLgFragPath =
    "./shaders/light.frag";  // light fragment shader
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

  Camera(const glm::vec3& pos, const glm::vec3& front, const glm::vec3& up,
         float speed)
      : pos(pos), front(front), up(up), speed(speed) {
    last_frame = glfwGetTime();
  }
};

struct RenderObject {
  class GeometryBuffer& geom_buff;
  class Shader& shader;
  glm::mat4& model;
};

render::ErrorType RenderTriangles(GLFWwindow* win,
                                  const std::vector<Triangle>& triangles);

void InitData(std::vector<float>& data, const std::vector<Triangle>& triangles);

void AddPointData(std::vector<float>& data, const Point& point,
                  const Color& color);

void RenderCycle(GLFWwindow* win, struct RenderObject& tr_obj);

Camera CameraSettings();

void CoordinateTransform(RenderObject& tr_obj, const Camera& camera);

void ProcessInput(GLFWwindow* win, Camera& camera);

//unsigned int LightSettings(unsigned vbo, unsigned int& light_vao);

#endif
