#include "triangles_rendering.hpp"

#include <cassert>
#include <memory>
#include <utility>
#include "glm/matrix.hpp"

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
  
  glm::vec3 color = glm::vec3(1.0F, 1.0F, 1.0F);
  glm::vec3 position = glm::vec3(0.8F, 0.0F, 0.2F);
  LightSource light_source(light_obj, color, position);

  RenderCycle(win, tr_obj, light_source);

  return render::ErrorType::kCorrect;
}

void InitData(std::vector<float>& data,
              const std::vector<Triangle>& triangles) {

  for (const auto& tr : triangles) {
    const Color& color = tr.color;
    glm::vec3 v_12 = glm::vec3(tr.p_1.x - tr.p_2.x, tr.p_1.y - tr.p_2.y,
                                     tr.p_1.z - tr.p_2.z);
    glm::vec3 v_13 = glm::vec3(tr.p_1.x - tr.p_3.x, tr.p_1.y - tr.p_3.y,
                                       tr.p_1.z - tr.p_3.z);
    glm::vec3 normal = glm::normalize(glm::cross(v_12, v_13));
    AddPointData(data, tr.p_1, color);
    AddNormalData(data, normal);
    AddPointData(data, tr.p_2, color);
    AddNormalData(data, normal);
    AddPointData(data, tr.p_3, color);
    AddNormalData(data, normal);
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

void AddNormalData(std::vector<float>& data, const glm::vec3& normal) {

  data.push_back(normal.x);
  data.push_back(normal.y);
  data.push_back(normal.z);
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

    DrawTriangles(tr_obj, camera, light_source);
 
    DrawLightSource(light_source, camera);

    glfwSwapBuffers(win);
    glfwPollEvents();

  }
}

void DrawTriangles(RenderObject& tr_obj, const Camera& camera, const LightSource& light_source) {

  tr_obj.shader->Use();
  tr_obj.geom_buff->Bind();
  UpdateTrPos(tr_obj, camera, light_source);
  tr_obj.geom_buff->Draw(GL_TRIANGLES); 
  tr_obj.geom_buff->Unbind();
  tr_obj.shader->Disable();
}

void DrawLightSource(LightSource& light_source, const Camera& camera) {

  light_source.light_obj.shader->Use();
  light_source.light_obj.geom_buff->Bind();
  UpdateLgPos(light_source, camera);
  light_source.light_obj.geom_buff->Draw(GL_TRIANGLES);
  light_source.light_obj.geom_buff->Unbind();
  light_source.light_obj.shader->Disable();
}

RenderObject CreateTriangleObj(std::vector<float>& raw_data, size_t triangles_number) {

  glm::mat4 model = glm::mat4(1.0F);  // init unit matrix
  glm::vec3 rotational_axis = glm::vec3(1.0F, 0.0F, 0.0F);
  model =
      glm::rotate(model, glm::radians(render::kRotateAngle), rotational_axis);
  
  glm::mat3 normal_mat = glm::mat3(glm::transpose(glm::inverse(model)));

  RenderObject tr_obj(
      std::make_unique<GeometryBuffer>(raw_data, render::kDimension,
                                        render::kColors, render::kNormal,
                                        triangles_number * render::kVertexes),
      std::make_unique<Shader>(render::kTrVertPath, render::kTrFragPath),
      model, normal_mat);
    
  tr_obj.geom_buff->Bind();
  tr_obj.geom_buff->SetCoordinates(render::kZeroLocation);
  tr_obj.geom_buff->SetColors(render::kFirstLocation);
  tr_obj.geom_buff->SetNormal(render::kSecondLocation);
  tr_obj.geom_buff->Unbind();

  return tr_obj;
}

RenderObject CreateLightObj() {
  
  std::vector<float> raw_data = {
    // Задняя грань (z = -0.075)
    -0.075f, -0.075f, -0.075f,
     0.075f, -0.075f, -0.075f,
     0.075f,  0.075f, -0.075f,
     0.075f,  0.075f, -0.075f,
    -0.075f,  0.075f, -0.075f,
    -0.075f, -0.075f, -0.075f,

    // Передняя грань (z = 0.075)
    -0.075f, -0.075f,  0.075f,
     0.075f, -0.075f,  0.075f,
     0.075f,  0.075f,  0.075f,
     0.075f,  0.075f,  0.075f,
    -0.075f,  0.075f,  0.075f,
    -0.075f, -0.075f,  0.075f,

    // Левая грань (x = -0.075)
    -0.075f,  0.075f,  0.075f,
    -0.075f,  0.075f, -0.075f,
    -0.075f, -0.075f, -0.075f,
    -0.075f, -0.075f, -0.075f,
    -0.075f, -0.075f,  0.075f,
    -0.075f,  0.075f,  0.075f,

    // Правая грань (x = 0.075)
     0.075f,  0.075f,  0.075f,
     0.075f,  0.075f, -0.075f,
     0.075f, -0.075f, -0.075f,
     0.075f, -0.075f, -0.075f,
     0.075f, -0.075f,  0.075f,
     0.075f,  0.075f,  0.075f,

    // Нижняя грань (y = -0.075)
    -0.075f, -0.075f, -0.075f,
     0.075f, -0.075f, -0.075f,
     0.075f, -0.075f,  0.075f,
     0.075f, -0.075f,  0.075f,
    -0.075f, -0.075f,  0.075f,
    -0.075f, -0.075f, -0.075f,

    // Верхняя грань (y = 0.075)
    -0.075f,  0.075f, -0.075f,
     0.075f,  0.075f, -0.075f,
     0.075f,  0.075f,  0.075f,
     0.075f,  0.075f,  0.075f,
    -0.075f,  0.075f,  0.075f,
    -0.075f,  0.075f, -0.075f 
  };

  glm::mat4 model = glm::mat4(1.0F);
  glm::mat3 normal_mat = glm::mat3(1.0F);
  
  RenderObject light_obj(
      std::make_unique<GeometryBuffer>(raw_data, render::kDimension, 0, 0, 36),
      std::make_unique<Shader>(render::kLgVertPath, render::kLgFragPath),
      model, normal_mat); 

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

void UpdateTrPos(RenderObject& tr_obj, const Camera& camera, const LightSource& light_source) {

  tr_obj.shader->SetMat4("model", tr_obj.model);
  tr_obj.shader->SetMat4("view", camera.view);
  tr_obj.shader->SetMat4("projection", camera.projection);
  tr_obj.shader->SetMat3("normalMatrix", tr_obj.normal_mat);
  tr_obj.shader->SetVecUniform("lightColor", light_source.color);
  tr_obj.shader->SetVecUniform("lightPos", light_source.position);
  tr_obj.shader->SetVecUniform("viewPos", camera.pos);
}

void UpdateLgPos(LightSource& light_source, const Camera& camera) {
  
  RenderObject& light_obj = light_source.light_obj;
  
  glm::mat4 model = glm::translate(light_obj.model, light_source.position);
  
  light_obj.shader->SetMat4("model", model);
  light_obj.shader->SetMat4("view", camera.view);
  light_obj.shader->SetMat4("projection", camera.projection);
  light_obj.shader->SetVecUniform("lightColor", light_source.color);
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
