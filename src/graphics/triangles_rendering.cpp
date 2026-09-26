#include "triangles_rendering.hpp"

#include <cassert>
#include <memory>
#include <utility>

namespace {
float last_mouse_x = 0.0F;
float last_mouse_y = 0.0F;
float sensitivity = 0.1F;
bool first_mouse = true;
}  // namespace

render::ErrorType RenderTriangles(GLFWwindow* win,
                                  const std::vector<Triangle>& triangles) {
  assert(win);

  int float_counter = (render::kDimension + render::kColors + render::kNormal) *
                      render::kVertexes;
  size_t triangles_number = triangles.size();
  size_t data_cap = (sizeof(float) * float_counter) * triangles_number;
  std::vector<float> raw_data;  // convert triangle data to float
  raw_data.reserve(data_cap);

  InitData(raw_data, triangles);
  
  RenderObject tr_obj = CreateTriangleObj(raw_data, triangles_number);
  RenderObject light_obj = CreateLightObj();
  
  glm::vec3 source_color = glm::vec3(1.0F, 0.0F, 0.0F);
  LightSource light_source(light_obj, source_color);

  RenderCycle(win, tr_obj, light_source);

  return render::ErrorType::kCorrect;
}

void InitData(std::vector<float>& data,
              const std::vector<Triangle>& triangles) {

  for (const auto& tr : triangles) {
    const Color& color = tr.color;
    AddPointData(data, tr.p_1, color);
    AddPointData(data, tr.p_2, color);
    AddPointData(data, tr.p_3, color);
  }
}

void AddPointData(std::vector<float>& data, const Point& point,
                  const Color& color) {
  data.push_back(point.x);
  data.push_back(point.y);
  data.push_back(point.z);
  data.push_back(color.r);
  data.push_back(color.g);
  data.push_back(color.b);
}

void RenderCycle(GLFWwindow* win, RenderObject& tr_obj, LightSource& light_source) {
  assert(win);

  float back_r = 0.0F, back_g = 0.0F, back_b = 0.0F, alpha = 1.0F;

  Camera camera = CameraSettings();

  glfwSetWindowUserPointer(win, &camera);  // save info about camera in window

  while (glfwGetKey(win, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
    glClearColor(back_r, back_g, back_b, alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ProcessInput(win, camera);

    tr_obj.shader->Use();
    tr_obj.geom_buff->Bind();
    UpdateTrPos(tr_obj, camera, light_source.source_color);
    tr_obj.geom_buff->Draw(GL_TRIANGLES); 
    tr_obj.geom_buff->Unbind();
    tr_obj.shader->Disable();

    light_source.light_obj.shader->Use();
    light_source.light_obj.geom_buff->Bind();
    UpdateLgPos(light_source, camera);
    light_source.light_obj.geom_buff->Draw(GL_TRIANGLES);
    light_source.light_obj.geom_buff->Unbind();
    light_source.light_obj.shader->Disable();

    glfwSwapBuffers(win);
    glfwPollEvents();

  }
}

RenderObject CreateTriangleObj(std::vector<float>& raw_data, size_t triangles_number) {

  glm::mat4 model = glm::mat4(1.0F);  // init unit matrix
  glm::vec3 rotational_axis = glm::vec3(1.0F, 0.0F, 0.0F);
  model =
      glm::rotate(model, glm::radians(render::kRotateAngle), rotational_axis);
  

  RenderObject tr_obj(
      std::make_unique<GeometryBuffer>(raw_data, render::kDimension,
                                        render::kColors, render::kNormal,
                                        triangles_number * render::kVertexes),
      std::make_unique<Shader>(render::kTrVertPath, render::kTrFragPath),
      model);
    
  tr_obj.geom_buff->Bind();
  tr_obj.geom_buff->SetCoordinates(render::kZeroLocation);
  tr_obj.geom_buff->SetColors(render::kFirstLocation);
  tr_obj.geom_buff->Unbind();

  return tr_obj;
}

RenderObject CreateLightObj() {
  
  std::vector<float> raw_data = {
    // Задняя грань
    -0.075f, -0.075f, -1.575f,
     0.075f, -0.075f, -1.575f,
     0.075f,  0.075f, -1.575f,
     0.075f,  0.075f, -1.575f,
    -0.075f,  0.075f, -1.575f,
    -0.075f, -0.075f, -1.575f,

    // Передняя грань
    -0.075f, -0.075f, -1.425f,
     0.075f, -0.075f, -1.425f,
     0.075f,  0.075f, -1.425f,
     0.075f,  0.075f, -1.425f,
    -0.075f,  0.075f, -1.425f,
    -0.075f, -0.075f, -1.425f,

    // Левая грань
    -0.075f,  0.075f, -1.425f,
    -0.075f,  0.075f, -1.575f,
    -0.075f, -0.075f, -1.575f,
    -0.075f, -0.075f, -1.575f,
    -0.075f, -0.075f, -1.425f,
    -0.075f,  0.075f, -1.425f,

    // Правая грань
     0.075f,  0.075f, -1.425f,
     0.075f,  0.075f, -1.575f,
     0.075f, -0.075f, -1.575f,
     0.075f, -0.075f, -1.575f,
     0.075f, -0.075f, -1.425f,
     0.075f,  0.075f, -1.425f,

    // Нижняя грань
    -0.075f, -0.075f, -1.575f,
     0.075f, -0.075f, -1.575f,
     0.075f, -0.075f, -1.425f,
     0.075f, -0.075f, -1.425f,
    -0.075f, -0.075f, -1.425f,
    -0.075f, -0.075f, -1.575f,

    // Верхняя грань
    -0.075f,  0.075f, -1.575f,
     0.075f,  0.075f, -1.575f,
     0.075f,  0.075f, -1.425f,
     0.075f,  0.075f, -1.425f,
    -0.075f,  0.075f, -1.425f,
    -0.075f,  0.075f, -1.575f

  };

  glm::mat4 model = glm::mat4(1.0F);
  
  RenderObject light_obj(
      std::make_unique<GeometryBuffer>(raw_data, render::kDimension, 0, 0, 36),
      std::make_unique<Shader>(render::kLgVertPath, render::kLgFragPath),
      model); 

  light_obj.geom_buff->Bind();
  light_obj.geom_buff->SetCoordinates(render::kZeroLocation);
  light_obj.geom_buff->Unbind();

  return light_obj;
}

void ProcessInput(GLFWwindow* win, Camera& camera) {
  assert(win);

  float current_time = glfwGetTime();
  camera.delta_time = current_time - camera.last_frame;
  camera.last_frame = current_time;
  float camera_speed = camera.speed * camera.delta_time;

  if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) {
    camera.pos += camera_speed * camera.front;
  }
  if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
    camera.pos -= camera_speed * camera.front;
  }
  if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
    camera.pos +=
        glm::normalize(glm::cross(camera.up, camera.front)) * camera_speed;
  }
  if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
    camera.pos -=
        glm::normalize(glm::cross(camera.up, camera.front)) * camera_speed;
  }
  
  camera.UpdateCameraMatrix();
}

Camera CameraSettings() {

  glm::vec3 camera_pos = glm::vec3(0.0F, 0.0F, 3.0F);
  glm::vec3 camera_front = glm::vec3(0.0, 0.0F, -1.0F);
  glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F);

  float base_speed = 2.5F;
  Camera camera(camera_pos, camera_front, up, base_speed);

  return camera;
}

void UpdateTrPos(RenderObject& tr_obj, const Camera& camera, const glm::vec3& lg_color) {

  tr_obj.shader->SetMatUniform("model", tr_obj.model);
  tr_obj.shader->SetMatUniform("view", camera.view);
  tr_obj.shader->SetMatUniform("projection", camera.projection);
  tr_obj.shader->SetVecUniform("lightColor", lg_color);
}

void UpdateLgPos(LightSource& light_source, const Camera& camera) {
  
  RenderObject& light_obj = light_source.light_obj;

  light_obj.shader->SetMatUniform("view", camera.view);
  light_obj.shader->SetMatUniform("projection", camera.projection);
  light_obj.shader->SetVecUniform("lightColor", light_source.source_color);
}

void MouseCallback(GLFWwindow* win, double xpos, double ypos) {
  assert(win);

  Camera* camera_ptr = static_cast<Camera*>(glfwGetWindowUserPointer(win));

  if (!camera_ptr) {
    return;
  }

  if (first_mouse) {
    last_mouse_x = xpos;
    last_mouse_y = ypos;
    first_mouse = false;
  }

  float delta_x = (xpos - last_mouse_x) * sensitivity;
  float delta_y = (last_mouse_y - ypos) * sensitivity;
  last_mouse_x = xpos;
  last_mouse_y = ypos;

  Camera& camera = *camera_ptr;
  camera.yaw += delta_x;
  camera.pitch += delta_y;

  if (camera.pitch > 89.0F) {
    camera.pitch = 89.0F;
  } else if (camera.pitch < -89.0F) {
    camera.pitch = -89.0F;
  }
  float pitch_rad = glm::radians(camera.pitch),
        yaw_rad = glm::radians(camera.yaw);
  camera.front.x = std::cos(pitch_rad) * std::cos(yaw_rad);
  camera.front.y = std::sin(pitch_rad);
  camera.front.z = std::cos(pitch_rad) * std::sin(yaw_rad);
}
