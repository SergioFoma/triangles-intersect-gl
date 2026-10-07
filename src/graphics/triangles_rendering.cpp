#include "triangles_rendering.hpp"
#include "geometry_buffer.hpp"
#include "utility.hpp"

#include "glm/matrix.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cassert>
#include <memory>
#include <utility>
#include <stdexcept>
#include <iostream>

namespace {
const int kDimension = 3;         // R^3 (x, y, z)
const int kVertexes = 3;          // Point p_1, p_2, p_3
const int kColors = 3;            // (r, g, b)
const int kNormal = 3;            // (n_x, n_y, n_z)
const int kSkyboxVer = 36;        // skybox vertexes number
const float kSpeedCoeff = 2.0F;
float last_mouse_x = 0.0F;
float last_mouse_y = 0.0F;
bool first_mouse = true;

unsigned int LoadCubemap(const render::SkyboxConfig& skybox_con) {

  unsigned int texture_id = 0;
  glGenTextures(render::kOneTexture, &texture_id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

  int width = 0, height = 0;
  int file_channels = 3;
  int level = 0, border = 0;

  const std::vector<std::string>& texture_sides = skybox_con.texture_sides;
  int sides_number = skybox_con.sides_number;

  for (int ind = 0; ind < sides_number; ++ind) {
    unsigned char* data = stbi_load(texture_sides[ind].c_str(), &width, &height,
                                   &file_channels, STBI_rgb);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + ind,
                 level, GL_RGB, width, height, border, GL_RGB,
                 GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  int error_code = glGetError();
  if (error_code) {
    throw std::runtime_error("LoadCubemap has an error!");
  }

  return texture_id;
}


render::SkyboxData CreateSkyboxObj(const std::vector<float>& raw_data,
                                     const render::SkyboxConfig& skybox_con) {

  glm::mat4 model = glm::mat4(1.0F);
  glm::mat4 normal_mat = glm::mat4(1.0F);

  unsigned int no_colors = 0, no_normals = 0;

  render::RenderObject skybox_obj = {
    GeometryBuffer(raw_data, kDimension,
                                     no_colors, no_normals,
                                     kSkyboxVer),
    Shader(skybox_con.shader_con.vert_path,
                             skybox_con.shader_con.frag_path),
                             model, normal_mat};

  skybox_obj.geom_buff.Bind();
  skybox_obj.geom_buff.SetCoordinates(skybox_con.shader_con.position_loc);
  skybox_obj.geom_buff.Unbind();

  unsigned int texture_id = LoadCubemap(skybox_con);

  skybox_obj.shader.Use();
  skybox_obj.shader.SetInt("skybox", 0);
  skybox_obj.shader.Disable();

  render::SkyboxData skybox_data = {skybox_con, std::move(skybox_obj), texture_id};

  int error_code = glGetError();
  if (error_code) {
    throw std::runtime_error("CreateSkyboxObj has an error!");
  }

  return skybox_data;
}


render::SkyboxData CallSkyboxCreating(const render::SkyboxConfig& skybox_con) {

  std::vector<float> skybox_vertices = {
        // positions
        -1.0F,  1.0F, -1.0F,
        -1.0F, -1.0F, -1.0F,
         1.0F, -1.0F, -1.0F,
         1.0F, -1.0F, -1.0F,
         1.0F,  1.0F, -1.0F,
        -1.0F,  1.0F, -1.0F,

        -1.0F, -1.0F,  1.0F,
        -1.0F, -1.0F, -1.0F,
        -1.0F,  1.0F, -1.0F,
        -1.0F,  1.0F, -1.0F,
        -1.0F,  1.0F,  1.0F,
        -1.0F, -1.0F,  1.0F,

         1.0F, -1.0F, -1.0F,
         1.0F, -1.0F,  1.0F,
         1.0F,  1.0F,  1.0F,
         1.0F,  1.0F,  1.0F,
         1.0F,  1.0F, -1.0F,
         1.0F, -1.0F, -1.0F,

        -1.0F, -1.0F,  1.0F,
        -1.0F,  1.0F,  1.0F,
         1.0F,  1.0F,  1.0F,
         1.0F,  1.0F,  1.0F,
         1.0F, -1.0F,  1.0F,
        -1.0F, -1.0F,  1.0F,

        -1.0F,  1.0F, -1.0F,
         1.0F,  1.0F, -1.0F,
         1.0F,  1.0F,  1.0F,
         1.0F,  1.0F,  1.0F,
        -1.0F,  1.0F,  1.0F,
        -1.0F,  1.0F, -1.0F,

        -1.0F, -1.0F, -1.0F,
        -1.0F, -1.0F,  1.0F,
         1.0F, -1.0F, -1.0F,
         1.0F, -1.0F, -1.0F,
        -1.0F, -1.0F,  1.0F,
         1.0F, -1.0F,  1.0F
  };

  return CreateSkyboxObj(skybox_vertices, skybox_con);
}

void UpdateTrPos(render::RenderObject& tr_obj, const render::Camera& camera,
                 const render::LightConfig& light_con) {

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
    throw std::runtime_error("UpdateTrPos has an error!");
  }
}

void UpdateSkybox(const render::RenderObject& skybox_obj, const render::Camera& camera) {

  glm::mat4 view = glm::mat4(glm::mat3(camera.view));

  skybox_obj.shader.SetMat4("view", view);
  skybox_obj.shader.SetMat4("projection", camera.projection);

  int error_code = glGetError();
  if (error_code) {
    throw std::runtime_error("UpdateSkybox has an error!");
  }
}

void DrawTriangles(render::RenderObject& tr_obj, const render::Camera& camera,
                   const render::LightConfig& light_con) {

  tr_obj.shader.Use();
  tr_obj.geom_buff.Bind();
  UpdateTrPos(tr_obj, camera, light_con);
  tr_obj.geom_buff.Draw(GL_TRIANGLES);
  tr_obj.geom_buff.Unbind();
  tr_obj.shader.Disable();

  int error_code = glGetError();
  if (error_code) {
    throw std::runtime_error("DrawTriangles has an error!");
  }
}

void DrawSkybox(render::SkyboxData& skybox_data,
                const render::Camera& camera) {

  render::RenderObject& skybox_obj = skybox_data.obj;

  skybox_obj.shader.Use();
  skybox_obj.geom_buff.Bind();
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_data.texture_id);
  UpdateSkybox(skybox_obj, camera);
  skybox_obj.geom_buff.Draw(GL_TRIANGLES);
  skybox_obj.geom_buff.Unbind();
  skybox_obj.shader.Disable();

  int error_code = glGetError();
  if (error_code) {
    throw std::runtime_error("DrawSkybox has an error!");
  }
}

void ProcessInput(GLFWwindow* win, render::Camera& camera) {
  assert(win);

  float speed_coeff = 1.0F;
  if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    speed_coeff = kSpeedCoeff;
  }

  float current_time = glfwGetTime();
  camera.delta_time = current_time - camera.last_frame;
  camera.last_frame = current_time;
  float camera_speed = speed_coeff * camera.speed * camera.delta_time;

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

  int error_code = glGetError();
  if (error_code) {
    throw std::runtime_error("ProcessInput has an error!");
  }
}


render::RenderObject CreateBoxObj(const std::vector<float>& data) {
  render::RenderObject obj{GeometryBuffer(data, 3, 3, 3, data.size() / 9),
    Shader("./shaders/box.vert", "./shaders/box.frag"), glm::mat4(1), glm::mat3(1)};
  obj.geom_buff.Bind();
  obj.geom_buff.SetCoordinates(0);
  obj.geom_buff.SetColors(1);
  obj.geom_buff.SetNormal(2);
  obj.geom_buff.Unbind();
  return obj;
}

void DrawBoxes(render::RenderObject& obj, const render::Camera& camera, bool morton_mode) {
  obj.shader.Use();
  obj.shader.SetInt("mortonMode", morton_mode);
  obj.shader.SetMat4("view", camera.view);
  obj.shader.SetMat4("projection", camera.projection);
  obj.geom_buff.Bind();
  obj.geom_buff.Draw(GL_LINES);
  obj.geom_buff.Unbind();
  obj.shader.Disable();
}

void RenderCycle(GLFWwindow* win, render::RenderObject& tr_obj,
                 const render::RenderConfig& render_con,
                 render::SkyboxData& skybox_data,
                 std::vector<render::RenderObject>& levels,
                 const std::vector<std::vector<float>>& box_levels) {
  assert(win);

  float back_r = 0.0F, back_g = 0.0F, back_b = 0.0F, alpha = 1.0F;

  render::Camera camera(render_con.camera_con);

  glfwSetWindowUserPointer(win, &camera);  // save info about camera in window

  bool show_boxes = true, show_triangles = true, morton_mode = true, show_path = true;
  size_t level = 0;
  bool previous_m = false, previous_p = false;
  bool previous_b = false, previous_t = false, previous_next = false, previous_back = false;
  auto update_title = [&] {
    const size_t count = levels.empty() ? 0 : box_levels[level].size() / (24 * 9);
    const std::string title = "Boxes level " + std::to_string(level) + "/" +
      std::to_string(levels.empty() ? 0 : levels.size() - 1) +
      " | " + std::to_string(count) + " boxes | " + (morton_mode ? "Morton: blue(high) -> red(low)" : "Level colors") +
      " | arrows: level | M: color | P: path | B/T: boxes/triangles | Esc";
    glfwSetWindowTitle(win, title.c_str());
  };
  std::vector<render::RenderObject> paths;
  paths.reserve(box_levels.size());
  for (const auto& data : box_levels) {
    std::vector<float> lines;
    std::vector<float> previous;
    for (size_t start = 0; start < data.size(); start += 24 * 9) {
      glm::vec3 low(data[start], data[start+1], data[start+2]), high = low;
      for (size_t j = start; j < start + 24 * 9; j += 9) {
        const glm::vec3 p(data[j], data[j+1], data[j+2]);
        low = glm::min(low, p); high = glm::max(high, p);
      }
      const auto center = (low + high) * 0.5F;
      std::vector<float> vertex{center.x, center.y, center.z,
        data[start+3], data[start+4], data[start+5],
        data[start+6], data[start+7], data[start+8]};
      if (!previous.empty()) {
        lines.insert(lines.end(), previous.begin(), previous.end());
        lines.insert(lines.end(), vertex.begin(), vertex.end());
      }
      previous = std::move(vertex);
    }
    paths.push_back(CreateBoxObj(lines));
  }
  update_title();
  while (!glfwWindowShouldClose(win) && glfwGetKey(win, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
    const bool m = glfwGetKey(win, GLFW_KEY_M) == GLFW_PRESS;
    const bool p = glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS;
    if (m && !previous_m) { morton_mode = !morton_mode; update_title(); }
    if (p && !previous_p) show_path = !show_path;
    previous_m = m;
    previous_p = p;
    const bool b = glfwGetKey(win, GLFW_KEY_B) == GLFW_PRESS;
    const bool t = glfwGetKey(win, GLFW_KEY_T) == GLFW_PRESS;
    const bool next = glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS ||
                      glfwGetKey(win, GLFW_KEY_G) == GLFW_PRESS;
    const bool back = glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS;
    if (b && !previous_b) show_boxes = !show_boxes;
    if (t && !previous_t) show_triangles = !show_triangles;
    if (next && !previous_next && level + 1 < levels.size()) { ++level; update_title(); }
    if (back && !previous_back && level > 0) { --level; update_title(); }
    previous_b = b;
    previous_t = t;
    previous_next = next;
    previous_back = back;
    glClearColor(back_r, back_g, back_b, alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ProcessInput(win, camera);

    if (show_triangles) DrawTriangles(tr_obj, camera, render_con.light_con);

    glDepthFunc(GL_LEQUAL);
    DrawSkybox(skybox_data, camera);
    glDepthFunc(GL_LESS);

    // Keep the wireframes visible even where triangles occlude their edges.
    glDisable(GL_DEPTH_TEST);
    if (show_boxes && !levels.empty()) DrawBoxes(levels[level], camera, morton_mode);
    if (show_path && morton_mode && !paths.empty()) DrawBoxes(paths[level], camera, true);
    glEnable(GL_DEPTH_TEST);


    glfwSwapBuffers(win);
    glfwPollEvents();

    if (glGetError()) {
      throw std::runtime_error("RenderCycle has an error!");
    }
  }
}

render::RenderObject CreateTriangleObj(const std::vector<float>& raw_data,
                                       const render::ShaderConfig& shader_con,
                                       size_t triangles_number) {

  glm::mat4 model = glm::mat4(1.0F);  // init unit matrix

  glm::mat3 normal_mat = glm::mat3(glm::transpose(glm::inverse(model)));

  render::RenderObject tr_obj = {
      GeometryBuffer(raw_data, kDimension,
                                        kColors, kNormal,
                                        triangles_number * kVertexes),
      Shader(shader_con.vert_path, shader_con.frag_path),
      model, normal_mat};

  tr_obj.geom_buff.Bind();
  tr_obj.geom_buff.SetCoordinates(shader_con.position_loc);
  tr_obj.geom_buff.SetColors(shader_con.color_loc);
  tr_obj.geom_buff.SetNormal(shader_con.normal_loc);
  tr_obj.geom_buff.Unbind();

  int error_code = glGetError();
  if (error_code) {
    throw std::runtime_error("CreateTriangleObj has an error!");
  }

  return tr_obj;
}
} // namespace

render::ErrorType render::RenderTriangles(GLFWwindow* win,
                                  size_t triangles_number,
                                  const std::vector<float>& triangles,
                                  const render::RenderConfig& render_con,
                                  const render::SkyboxConfig& skybox_con,
                                  const std::vector<std::vector<float>>& box_levels) {
  assert(win);

  render::RenderObject tr_obj = CreateTriangleObj(triangles, render_con.shader_con,
                                                  triangles_number);
  render::SkyboxData skybox_data = CallSkyboxCreating(skybox_con);

  std::vector<render::RenderObject> levels;
  levels.reserve(box_levels.size());
  for (const auto& data : box_levels) levels.push_back(CreateBoxObj(data));
  auto fitted = render_con;
  if (!triangles.empty()) {
    glm::vec3 low(triangles[0], triangles[1], triangles[2]), high = low;
    for (size_t i = 0; i < triangles.size(); i += 9) {
      const glm::vec3 point(triangles[i], triangles[i + 1], triangles[i + 2]);
      low = glm::min(low, point);
      high = glm::max(high, point);
    }
    const glm::vec3 center = (low + high) * 0.5F;
    const float radius = std::max(glm::length(high - low) * 0.5F, 0.1F);
    const float angle = glm::radians(fitted.camera_con.fovy) * 0.5F;
    const float limiting_angle = std::min(angle, std::atan(std::tan(angle) * fitted.camera_con.aspect));
    const float distance = radius / std::sin(limiting_angle) * 1.15F;
    fitted.camera_con.pos = center + glm::vec3(0, 0, distance);
    fitted.camera_con.near = std::max(radius * 0.001F, 0.0001F);
    fitted.camera_con.far = distance + radius * 10.0F;
    fitted.camera_con.speed = radius;
  }
  std::cout << "Left/Right: box level (0 = triangles, last = entire scene).\n"
               "M: Morton/level colors; P: Morton path; B: boxes; T: triangles.\n"
               "Move: WASD, mouse; Shift: faster; Esc: close.\n";
  RenderCycle(win, tr_obj, fitted, skybox_data, levels, box_levels);

  if (glGetError()) {
    throw std::runtime_error("RenderTriangles has an error!");
    return render::ErrorType::kError;
  }

  return render::ErrorType::kCorrect;
}

void utility::detail::MouseCallback(GLFWwindow* win, double xpos, double ypos) {
  assert(win);

  render::Camera* camera_ptr = static_cast<render::Camera*>(glfwGetWindowUserPointer(win));

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

  render::Camera& camera = *camera_ptr;
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

