
#include "configs.hpp"

config::LightConfig config::UserConfigs::CreateLightConfig() {
  glm::vec3 color = glm::vec3(1.0F);
  float cut_off = 12.5F;
  float outer_cut_off = 17.5F;
  float constant = 1.0F;
  float linear = 0.0014F;
  float quadratic = 0.0007F;

  LightConfig light = {color,    cut_off, outer_cut_off,
                       constant, linear,  quadratic};

  return light;
}

config::CameraConfig config::UserConfigs::CreateCameraConfig(const config::WinConfig& win_con) {
  glm::vec3 pos = glm::vec3(0.0F, 0.0F, 3.0F);
  glm::vec3 front = glm::vec3(0.0F, 0.0F, -1.0F);
  glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F);
  float speed = 3.0F;
  float aspect =
      static_cast<float>(win_con.width) / static_cast<float>(win_con.height);
  float fovy = 45.0F;
  float near_plane = 0.1F;
  float far_plane = 100.0F;
  float mouse_sensitivity = 0.1F;

  CameraConfig camera = {pos,        front,     up,
                         speed,      aspect,    fovy,
                         near_plane, far_plane, mouse_sensitivity};

  return camera;
}

config::ShaderConfig config::UserConfigs::CreateShaderConfig(const resource::Resource& resource,
                                        const std::string& vert_sh,
                                        const std::string& frag_sh) {
  int position_loc = 0;  // location in shaders
  int color_loc = 1;     // location in shaders
  int normal_loc = 2;    // location in shaders

  ShaderConfig shader_con = {vert_sh, frag_sh, position_loc, color_loc,
                             normal_loc};
  return shader_con;
}

config::SkyboxConfig config::UserConfigs::CreateSkyboxConfig(const resource::Resource& resource) {

  unsigned int sides_number = 6;
  unsigned int triangles_number = 36;

  ShaderConfig shader_con = {resource.skybox_vert, resource.skybox_frag,
                             0, 1, 2};

  SkyboxConfig skybox_con = {resource.texture_sides, sides_number,
                             triangles_number, shader_con};

  return skybox_con;
}

config::WinConfig config::UserConfigs::CreateWinConfig() {
  int width = 1000;
  int height = 800;
  int major_version = 4;
  int minor_version = 1;

  WinConfig win_con = {width, height, major_version, minor_version};

  return win_con;
}
