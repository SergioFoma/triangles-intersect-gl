#include "triangles_rendering.hpp"

#include "glm/matrix.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cassert>
#include <memory>
#include <utility>

namespace {
const int kDimension = 3;         // R^3 (x, y, z)
const int kVertexes = 3;          // Point p_1, p_2, p_3
const int kColors = 3;            // (r, g, b)
const int kNormal = 3;            // (n_x, n_y, n_z)
const int kSkyboxVer = 36;        // skybox vertexes number
float last_mouse_x = 0.0F;
float last_mouse_y = 0.0F;
bool first_mouse = true;
} // namespace

render::ErrorType RenderTriangles(GLFWwindow* win,
                                  const std::vector<render::Triangle>& triangles,
                                  const render::RenderConfig& render_con,
                                  const render::SkyboxConfig& skybox_con) {
  assert(win);

  int float_counter = (kDimension + kColors + kNormal) * kVertexes;
  size_t triangles_number = triangles.size();
  size_t data_cap = (sizeof(float) * float_counter) * triangles_number;
  std::vector<float> raw_data;  // convert triangle data to float
  raw_data.reserve(data_cap);

  InitData(raw_data, triangles);
  
  render::RenderObject tr_obj = CreateTriangleObj(raw_data, render_con.shader_con,
                                                  triangles_number);
  render::SkyboxData skybox_data = CallSkyboxCreating(skybox_con);

  RenderCycle(win, tr_obj, render_con, skybox_data);

  return render::ErrorType::kCorrect;
}

void InitData(std::vector<float>& data,
              const std::vector<render::Triangle>& triangles) {

  for (const auto& tr : triangles) {
    const render::Color& color = tr.color;
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

void AddPointData(std::vector<float>& data, const render::Point& point,
                  const render::Color& color) {
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

void RenderCycle(GLFWwindow* win, render::RenderObject& tr_obj,
                 const render::RenderConfig& render_con,
                 render::SkyboxData& skybox_data) {
  assert(win);

  float back_r = 0.0F, back_g = 0.0F, back_b = 0.0F, alpha = 1.0F;

  render::Camera camera(render_con.camera_con);

  glfwSetWindowUserPointer(win, &camera);  // save info about camera in window
    
  while (glfwGetKey(win, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
    glClearColor(back_r, back_g, back_b, alpha);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ProcessInput(win, camera);
  
    DrawTriangles(tr_obj, camera, render_con.light_con);

    glDepthFunc(GL_LEQUAL);
    DrawSkybox(skybox_data, camera);
    glDepthFunc(GL_LESS);


    glfwSwapBuffers(win);
    glfwPollEvents();

  }
}

void DrawTriangles(render::RenderObject& tr_obj, const render::Camera& camera,
                   const render::LightConfig& light_con) {

  tr_obj.shader->Use();
  tr_obj.geom_buff->Bind();
  UpdateTrPos(tr_obj, camera, light_con);
  tr_obj.geom_buff->Draw(GL_TRIANGLES); 
  tr_obj.geom_buff->Unbind();
  tr_obj.shader->Disable();
}

void DrawSkybox(render::SkyboxData& skybox_data,
                const render::Camera& camera) {
  
  render::RenderObject& skybox_obj = skybox_data.obj;

  skybox_obj.shader->Use();
  skybox_obj.geom_buff->Bind();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_data.texture_id);
  UpdateSkybox(skybox_obj, camera);
  skybox_obj.geom_buff->Draw(GL_TRIANGLES);
  skybox_obj.geom_buff->Unbind();
  skybox_obj.shader->Disable();
}

render::RenderObject CreateTriangleObj(const std::vector<float>& raw_data,
                                       const render::ShaderConfig& shader_con,
                                       size_t triangles_number) {

  glm::mat4 model = glm::mat4(1.0F);  // init unit matrix

  glm::mat3 normal_mat = glm::mat3(glm::transpose(glm::inverse(model)));

  render::RenderObject tr_obj = {
      std::make_unique<GeometryBuffer>(raw_data, kDimension,
                                        kColors, kNormal,
                                        triangles_number * kVertexes),
      std::make_unique<Shader>(shader_con.vert_path, shader_con.frag_path),
      model, normal_mat};
    
  tr_obj.geom_buff->Bind();
  tr_obj.geom_buff->SetCoordinates(shader_con.position_loc);
  tr_obj.geom_buff->SetColors(shader_con.color_loc);
  tr_obj.geom_buff->SetNormal(shader_con.normal_loc);
  tr_obj.geom_buff->Unbind();

  return tr_obj;
}

render::SkyboxData CreateSkyboxObj(const std::vector<float>& raw_data,
                                     const render::SkyboxConfig& skybox_con) {

  glm::mat4 model = glm::mat4(1.0F);
  glm::mat4 normal_mat = glm::mat4(1.0F);
  
  unsigned int no_colors = 0, no_normals = 0;

  render::RenderObject skybox_obj = {
    std::make_unique<GeometryBuffer>(raw_data, kDimension,
                                     no_colors, no_normals,
                                     kSkyboxVer),
    std::make_unique<Shader>(skybox_con.shader_con.vert_path,
                             skybox_con.shader_con.frag_path),
                             model, normal_mat};

  skybox_obj.geom_buff->Bind();
  skybox_obj.geom_buff->SetCoordinates(skybox_con.shader_con.position_loc);
  skybox_obj.geom_buff->Unbind();

  unsigned int texture_id = LoadCubemap(skybox_con);

  skybox_obj.shader->SetInt("skybox", 0);

  render::SkyboxData skybox_data = {skybox_con, std::move(skybox_obj), texture_id};

  return skybox_data;
}

unsigned int LoadCubemap(const render::SkyboxConfig& skybox_con) {
  const int one_texture = 1;

  unsigned int texture_id = 0;
  glGenTextures(one_texture, &texture_id);
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
  
  return texture_id;
}

void ProcessInput(GLFWwindow* win, render::Camera& camera) {
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

void UpdateTrPos(render::RenderObject& tr_obj, const render::Camera& camera,
                 const render::LightConfig& light_con) {

  tr_obj.shader->SetMat4("model", tr_obj.model);
  tr_obj.shader->SetMat4("view", camera.view);
  tr_obj.shader->SetMat4("projection", camera.projection);
  tr_obj.shader->SetMat3("normalMatrix", tr_obj.normal_mat);
  tr_obj.shader->SetVec3("light.lightColor", light_con.color);
  tr_obj.shader->SetVec3("light.viewPos", camera.pos);
  tr_obj.shader->SetVec3("light.direction", camera.front);

  tr_obj.shader->SetFloat("light.constant", light_con.constant);
  tr_obj.shader->SetFloat("light.linear", light_con.linear);
  tr_obj.shader->SetFloat("light.quadratic", light_con.quadratic);
  tr_obj.shader->SetFloat("light.cutOff", glm::cos(glm::radians(light_con.cut_off)));
  tr_obj.shader->SetFloat("light.outerCutOff", glm::cos(glm::radians(light_con.outer_cut_off)));
}

void UpdateSkybox(const render::RenderObject& skybox_obj, const render::Camera& camera) {
  
  glm::mat4 view = glm::mat4(glm::mat3(camera.view));

  skybox_obj.shader->SetMat4("view", view);
  skybox_obj.shader->SetMat4("projection", camera.projection);
}

void MouseCallback(GLFWwindow* win, double xpos, double ypos) {
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
