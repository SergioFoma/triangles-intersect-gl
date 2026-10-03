#include "triangles_rendering.hpp"
#include "utility.hpp"

#include <iostream>

namespace {
render::LightConfig CreateLightConfig() {

  glm::vec3 color = glm::vec3(1.0F);
  float cut_off = 12.5F;
  float outer_cut_off = 17.5F;
  float constant = 1.0F;
  float linear = 0.09F;
  float quadratic = 0.032F;

  render::LightConfig light = {color, cut_off, outer_cut_off,
                       constant, linear, quadratic};

  return light;
}

render::CameraConfig CreateCameraConfig(const utility::WinConfig& win_con) {
  glm::vec3 pos = glm::vec3(0.0F, 0.0F, 3.0F);
  glm::vec3 front = glm::vec3(0.0F, 0.0F, -1.0F);
  glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F);
  float speed = 2.5F;
  float aspect = static_cast<float>(win_con.width) / static_cast<float>(win_con.height);
  float fovy = 45.0F;
  float near = 0.1F;
  float far = 100.0F;
  float mouse_sensitivity = 0.1F;

  render::CameraConfig camera = {pos, front, up, speed, aspect,
                         fovy, near, far, mouse_sensitivity};

  return camera;
}

render::ShaderConfig CreateShaderConfig() {
  std::string vert_path =
    "./shaders/triangle.vert";    // triangle vertex shader
  std::string frag_path =
    "./shaders/triangle.frag";    // triangle fragment shader
  int position_loc = 0;               // location in shaders
  int color_loc = 1;                  // location in shaders
  int normal_loc = 2;                 // location in shaders

  render::ShaderConfig shader_con = {vert_path, frag_path,
                                  position_loc, color_loc, normal_loc};
  return shader_con;
}

render::SkyboxConfig CreateSkyboxConfig() {
  std::vector<std::string> texture_sides = {
    "./blue/bkg1_right.png",
    "./blue/bkg1_left.png",
    "./blue/bkg1_top.png",
    "./blue/bkg1_bot.png",
    "./blue/bkg1_back.png",
    "./blue/bkg1_front.png"
  };

  unsigned int sides_number = 6;
  unsigned int triangles_number = 36;

  render::ShaderConfig shader_con = {"./shaders/skybox.vert", "./shaders/skybox.frag",
                                      0, 1, 2};

  render::SkyboxConfig skybox_con = {texture_sides, sides_number,
                       triangles_number,
                       shader_con};

  return skybox_con;
}

utility::WinConfig CreateWinConfig() {
  int width = 1000;
  int height = 800;
  int major_version = 4;
  int minor_version = 1;

  utility::WinConfig win_con = {width, height, major_version, minor_version};

  return win_con;
}
} // namespace

int main() {

  utility::WinConfig win_con = CreateWinConfig();
  render::LightConfig light_con = CreateLightConfig();
  render::CameraConfig camera_con = CreateCameraConfig(win_con);
  render::ShaderConfig shader_config = CreateShaderConfig();
  render::SkyboxConfig skybox_config = CreateSkyboxConfig();
  render::RenderConfig render_con = {camera_con, light_con, shader_config};

  utility::InitOpenGl(win_con);

  GLFWwindow* win = utility::CreateWindow("triangle 3D", win_con);
  if (!win) {
    std::cout << "CreateWindow: returned null ptr!\n";
    utility::CleanResources();
    return 0;
  }

  if (utility::InitGlad() != utility::ErrorType::kCorrect) {
    std::cout << "InitGlad: returned negative value!\n";
    utility::CleanResources();
    return 0;
  }

  utility::ConfigureViewing(win);

  render::Point p_1_1 = {-0.5F, -0.5F, 0.0F};
  render::Point p_1_2 = {0.5F, -0.5F, 0.0F};
  render::Point p_1_3 = {0.0F, 0.5F, 0.0F};
  render::Color color_1 = {1.0F, 0.5F, 0.31F};

  render::Point p_2_1 = {0.4F, 0.1F, -0.1F};
  render::Point p_2_2 = {0.9F, 0.4F, -0.3F};
  render::Point p_2_3 = {0.7F, 0.5F, -0.1F};
  render::Color color_2 = {0.5F, 0.5F, 0.5F};

  render::Triangle tr_1(p_1_1, p_1_2, p_1_3, color_1);
  render::Triangle tr_2(p_2_1, p_2_2, p_2_3, color_2);

  /*
  std::vector<render::Triangle> triangles = {tr_1, tr_2};

  render::ErrorType error_code = render::RenderTriangles(win, triangles, render_con, skybox_config);
  if (error_code != render::ErrorType::kCorrect) {
    std::cout << "RenderTriangles: return negative error code!\n";
  }
  */

  utility::CleanResources();

  return 0;
}
