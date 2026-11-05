#ifndef CONFIGS_HPP_
#define CONFIGS_HPP_

#include <glm/glm.hpp>

#include "resources.hpp"

// ==================== Configs For User ==========================

namespace config {

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

struct WinConfig {
  int width = 0;
  int height = 0;
  int major_version = 0;
  int minor_version = 0;
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

struct UserConfigs {
  config::WinConfig win_con;
  config::LightConfig light_con;
  config::CameraConfig camera_con;
  config::ShaderConfig tr_shader_con;
  config::ShaderConfig point_shader_con;
  config::ShaderConfig line_shader_con;
  config::SkyboxConfig skybox_con;
  config::RenderConfig render_con;

  UserConfigs(const std::string& resource_paths, const char* bin_paths) {
    std::string target_paths;

    if (resource_paths.empty()) {
      target_paths = resource::Resource::GetDefaultPath(bin_paths);
    } else {
      target_paths = resource_paths;
    }

    const resource::TextureMap& texture_map = resource::CreateTextureMap();
    resource::Resource resource(target_paths, texture_map);

    win_con = CreateWinConfig();
    light_con = CreateLightConfig();
    camera_con = CreateCameraConfig(win_con);
    tr_shader_con = CreateShaderConfig(
        resource, resource.triangle_vert, resource.triangle_frag);
    point_shader_con =
        CreateShaderConfig(resource, resource.point_vert, resource.point_frag);
    line_shader_con =
        CreateShaderConfig(resource, resource.line_vert, resource.line_frag);
    skybox_con = CreateSkyboxConfig(resource);
    render_con = {camera_con, light_con, tr_shader_con,
                                      point_shader_con, line_shader_con};
  }

 private:
  LightConfig CreateLightConfig();

  CameraConfig CreateCameraConfig(const WinConfig& win_con);

  ShaderConfig CreateShaderConfig(const resource::Resource& resource,
                                        const std::string& vert_sh,
                                        const std::string& frag_sh);

  SkyboxConfig CreateSkyboxConfig(const resource::Resource& resource);

  WinConfig CreateWinConfig();
};
} // namespace config

#endif
