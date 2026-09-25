#include "triangles_rendering.hpp"

#include <cassert>

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

  GeometryBuffer* geom_buff = new GeometryBuffer(
      raw_data, render::kDimension, render::kColors, render::kNormal,
      triangles_number * render::kDimension);
  assert(geom_buff);

  Shader* tr_shader = new Shader(render::kTrVertPath, render::kTrFragPath);
  assert(tr_shader);

  Shader* light_shader = new Shader(render::kLgVertPath, render::kLgFragPath);
  assert(light_shader);

  // ============== TRIANGLE SETTING ========================
  glm::mat4 model = glm::mat4(1.0F);  // init unit matrix
  glm::vec3 rotational_axis = glm::vec3(1.0F, 0.0F, 0.0F);
  model =
      glm::rotate(model, glm::radians(render::kRotateAngle), rotational_axis);

  RenderObject tr_obj(*geom_buff, *tr_shader, model);

  tr_obj.geom_buff.Bind();
  tr_obj.geom_buff.SetCoordinates(render::kZeroLocation);
  tr_obj.geom_buff.SetColors(render::kFirstLocation);
  tr_obj.geom_buff.Unbind();

  // ========================================================

  RenderCycle(win, tr_obj);

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

void RenderCycle(GLFWwindow* win, struct RenderObject& tr_obj) {
  assert(win);

  float back_r = 0.0F, back_g = 0.0F, back_b = 0.0F, alpha = 1.0F;

  Camera camera = CameraSettings();

  glfwSetWindowUserPointer(win, &camera);  // save info about camera in window

  while (glfwGetKey(win, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
    glClearColor(back_r, back_g, back_b, alpha);
    glClear(GL_COLOR_BUFFER_BIT);

    tr_obj.shader.Use();
    tr_obj.geom_buff.Bind();

    ProcessInput(win, camera);

    CoordinateTransform(tr_obj, camera);
    tr_obj.geom_buff.Draw(GL_TRIANGLES);

    glfwSwapBuffers(win);
    glfwPollEvents();

    tr_obj.geom_buff.Unbind();
    tr_obj.shader.Disable();
  }
}

/*
unsigned int LightingSettings(unsigned int vbo, unsigned int& light_vao) {
    
    std::string light_vert_str = ReadShader(render::kLgVertPath);
    std::string light_frag_str = ReadShader(render::kLgFragPath);

    const char* light_vert_ptr = light_vert_str.c_str();
    const char* light_frag_ptr = light_frag_str.c_str();
        
    unsigned int light_shader = LinkShaders(light_vert_ptr, light_frag_ptr);

    unsigned int light_vao = 0;
    glGenVertexArrays(render::kBuffCount, &light_vao);
    glBindVertexArray(light_vao);
    
    int float_offset = (2 * render::kDimension) * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(render::kZeroLocation, render::kDimension, GL_FLOAT,
                          GL_FALSE, float_offset, (void*)0);

    glEnableVertexAttribArray(render::kZeroLocation);

    return light_shader;
}
*/

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
}

Camera CameraSettings() {

  glm::vec3 camera_pos = glm::vec3(0.0F, 0.0F, 3.0F);
  glm::vec3 camera_front = glm::vec3(0.0, 0.0F, -1.0F);
  glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F);

  float base_speed = 2.5F;
  Camera camera(camera_pos, camera_front, up, base_speed);

  return camera;
}

void CoordinateTransform(RenderObject& tr_obj, const Camera& camera) {

  glm::mat4 view =
      glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);

  glm::mat4 projection =
      glm::perspective(glm::radians(render::kFovy), render::kAspect,
                       render::kNear, render::kFar);

  tr_obj.shader.SetUniform("model", tr_obj.model);
  tr_obj.shader.SetUniform("view", view);
  tr_obj.shader.SetUniform("projection", projection);
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
