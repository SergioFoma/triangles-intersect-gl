#include "triangles_rendering.hpp"
#include "geometry_buffer.hpp"
#include "graphics_container.hpp"
#include "utility.hpp"

#include "glm/matrix.hpp"

#include <cassert>
#include <memory>
#include <utility>
#include <stdexcept>
#include <iostream>

#include "shapes.hpp"

namespace {
const float kSpeedCoeff = 2.0F;
float last_mouse_x = 0.0F;
float last_mouse_y = 0.0F;
bool first_mouse = true;

void UpdateShapePos(graphics::RenderObject& tr_obj, const graphics::Camera& camera,
                 const config::LightConfig& light_con) {

  tr_obj.shader.SetMat4("model", tr_obj.model);
  tr_obj.shader.SetMat4("view", camera.view);
  tr_obj.shader.SetMat4("projection", camera.projection);
  tr_obj.shader.SetMat3("normalMatrix", tr_obj.normal_mat);
  tr_obj.shader.SetVec3("light.lightColor", light_con.color);
  tr_obj.shader.SetVec3("light.viewPos", camera.pos);
  tr_obj.shader.SetVec3("light.direction", camera.front);

  tr_obj.shader.SetFloat("light.constant", light_con.constant);
  tr_obj.shader.SetFloat("light.linear", light_con.linear);
  tr_obj.shader.SetFloat("light.quadratic", light_con.quadratic);
  tr_obj.shader.SetFloat("light.cutOff", glm::cos(glm::radians(light_con.cut_off)));
  tr_obj.shader.SetFloat("light.outerCutOff", glm::cos(glm::radians(light_con.outer_cut_off)));

  int error_code = glGetError();
  if (error_code) {
   prog_error::ErrorInfo inf = {prog_error::ErrorCode::kRendering,
                                "Error of updating shape pos",
                                "UpdateTrPos"};
    throw prog_error::ProgError(std::move(inf));
  }
}

void UpdateSkybox(const graphics::RenderObject& skybox_obj, const graphics::Camera& camera) {

  glm::mat4 view = glm::mat4(glm::mat3(camera.view));

  skybox_obj.shader.SetMat4("view", view);
  skybox_obj.shader.SetMat4("projection", camera.projection);

  int error_code = glGetError();
  if (error_code) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kRendering,
                                   "Error of Uodate skybox",
                                   "UodateSkybox"};
    throw prog_error::ProgError(std::move(inf));
  }
}

void DrawShape(graphics::RenderObject& shape_obj, const graphics::Camera& camera,
               const config::LightConfig& light_con, GLenum mode) {

  shape_obj.shader.Use();
  shape_obj.geom_buff.Bind();
  UpdateShapePos(shape_obj, camera, light_con);
  shape_obj.geom_buff.Draw(mode, shape_obj.start_pos, shape_obj.count);
  shape_obj.geom_buff.Unbind();
  shape_obj.shader.Disable();

  int error_code = glGetError();
  if (error_code) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kRendering,
                                   "Error of Draw shape",
                                   "DrawTriangles"};
    throw prog_error::ProgError(std::move(inf));
  }
}

void DrawSkybox(graphics::SkyboxData& skybox_data,
                const graphics::Camera& camera) {

  graphics::RenderObject& skybox_obj = skybox_data.obj;

  skybox_obj.shader.Use();
  skybox_obj.geom_buff.Bind();
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_data.GetTextureId());
  UpdateSkybox(skybox_obj, camera);
  skybox_obj.geom_buff.Draw(GL_TRIANGLES, skybox_obj.start_pos, skybox_obj.count);
  skybox_obj.geom_buff.Unbind();
  skybox_obj.shader.Disable();

  int error_code = glGetError();
  if (error_code) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kRendering,
                                   "Error of draw skybox",
                                   "DrawSkybox"};
    throw prog_error::ProgError(std::move(inf));
  }
}

void MoveCamera(graphics::Camera& camera, utility::Window& win, float camera_speed) {
 if (glfwGetKey(win.GetPtr(), GLFW_KEY_W) == GLFW_PRESS) {
    camera.pos += camera_speed * camera.front;
  }
  if (glfwGetKey(win.GetPtr(), GLFW_KEY_S) == GLFW_PRESS) {
    camera.pos -= camera_speed * camera.front;
  }
  if (glfwGetKey(win.GetPtr(), GLFW_KEY_A) == GLFW_PRESS) {
    camera.pos +=
        glm::normalize(glm::cross(camera.up, camera.front)) * camera_speed;
  }
  if (glfwGetKey(win.GetPtr(), GLFW_KEY_D) == GLFW_PRESS) {
    camera.pos -=
        glm::normalize(glm::cross(camera.up, camera.front)) * camera_speed;
  }
}

void ProcessInput(utility::Window& win, graphics::Camera& camera) {
  float speed_coeff = 1.0F;
  if (glfwGetKey(win.GetPtr(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    speed_coeff = kSpeedCoeff;
  }

  float current_time = glfwGetTime();
  camera.delta_time = current_time - camera.last_frame;
  camera.last_frame = current_time;
  float camera_speed = speed_coeff * camera.speed * camera.delta_time;

  MoveCamera(camera, win, camera_speed);

  static bool old_key_f_status = false;
  bool current_key_f_status = (glfwGetKey(win.GetPtr(), GLFW_KEY_F) == GLFW_PRESS);
  if (!old_key_f_status && current_key_f_status) {
    std::pair<int, int> win_size = win.Resize();
    camera.SetPerspectiveMatrix(static_cast<float>(win_size.first)/
                                static_cast<float>(win_size.second));
    first_mouse = true;
  }
  old_key_f_status = current_key_f_status;

  camera.UpdateCameraMatrix();

  if (glGetError()) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kRendering,
                                   "Error of parse input data",
                                   "ProcessInput"};
    throw prog_error::ProgError(std::move(inf));
  }
}


void RenderCycle(utility::Window& win,
                 graphics::GraphicsContainer& graphics_container,
                 const config::UserConfigs& configs) {

  float black_r = 0.0F, black_g = 0.0F, black_b = 0.0F, alpha = 1.0F;

  graphics::ShapesObject& shapes_obj = graphics_container.GetShapesObj();
  graphics::Camera& camera = graphics_container.GetCamera();

  glfwSetWindowUserPointer(win.GetPtr(), &camera);  // save info about camera in window

  glClearColor(black_r, black_g, black_b, alpha);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ProcessInput(win, camera);
  DrawShape(shapes_obj.tr_obj, camera, configs.light_con, GL_TRIANGLES);
  DrawShape(shapes_obj.point_obj, camera, configs.light_con, GL_POINTS);
  DrawShape(shapes_obj.line_obj, camera, configs.light_con, GL_LINES);
  glDepthFunc(GL_LEQUAL);
  DrawSkybox(graphics_container.GetSkybox(), camera);
  glDepthFunc(GL_LESS);

  glfwSwapBuffers(win.GetPtr());
  glfwPollEvents();

  if (glGetError()) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kRendering,
                                "Error of redeting cycle",
                                "RenderCycle"};
    throw prog_error::ProgError(std::move(inf));
  }
}

} // namespace

render::ErrorType render::RenderTriangles(utility::Window& win,
                                  video_buff::BufferManager& buff_manager,
                                  graphics::GraphicsContainer& graphics_container,
                                  const config::UserConfigs& configs,
                                  const std::vector<float>& triangles) {

  video_buff::GeometryBuffer& video_buff = buff_manager.GetVideoBuff();
  video_buff.LoadData(triangles);

  RenderCycle(win, graphics_container, configs);

  if (glGetError()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kRendering,
                                   "Error of rendering Triangles",
                                   "RenderTriangles"};
      throw prog_error::ProgError(std::move(inf));
  }

  return render::ErrorType::kCorrect;
}

void utility::detail::MouseCallback(GLFWwindow* win, double xpos, double ypos) {
  assert(win);

  graphics::Camera* camera_ptr = static_cast<graphics::Camera*>(glfwGetWindowUserPointer(win));

  if (!camera_ptr) {
    return;
  }

  if (first_mouse) {
    last_mouse_x = xpos;
    last_mouse_y = ypos;
    first_mouse = false;
  }

  float delta_x = (xpos - last_mouse_x) * camera_ptr->sensitivity;
  float delta_y = (last_mouse_y - ypos) * camera_ptr->sensitivity;
  last_mouse_x = xpos;
  last_mouse_y = ypos;

  graphics::Camera& camera = *camera_ptr;
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
