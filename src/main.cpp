#include <CLI/CLI.hpp>

#include "adapter.hpp"
#include "utility.hpp"
#include "read_data.hpp"

namespace {

render::LightConfig CreateLightConfig() {
  glm::vec3 color = glm::vec3(1.0F);
  float cut_off = 12.5F;
  float outer_cut_off = 17.5F;
  float constant = 1.0F;
  float linear = 0.0014F;
  float quadratic = 0.0007F;

  render::LightConfig light = {color, cut_off, outer_cut_off,
                      constant, linear, quadratic};

  return light;
}

render::CameraConfig CreateCameraConfig(const utility::WinConfig& win_con) {
  glm::vec3 pos = glm::vec3(0.0F, 0.0F, 3.0F);
  glm::vec3 front = glm::vec3(0.0F, 0.0F, -1.0F);
  glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F);
  float speed = 3.0F;
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

int main(int argc, char** argv) {

  CLI::App app("Finding triangles intersection and rendering them");
  argv = app.ensure_utf8(argv);
  std::string input_path;
  app.add_option("-f,--input_file,input_name", input_path, "The input data file");
  bool is_only_intersect = false;
  app.add_flag("--only-intersection-analysis", is_only_intersect, "Analyzes only intersection");
  CLI11_PARSE(app, argc, argv);

  analyzer::TriangleArr triangles = reader::ReadData(input_path);
  size_t triangles_number = triangles.size();
  analyzer::InterAnalyzer analyzer(std::move(triangles));

  if (!is_only_intersect) {
    utility::WinConfig win_con = CreateWinConfig();
    render::LightConfig light_con = CreateLightConfig();
    render::CameraConfig camera_con = CreateCameraConfig(win_con);
    render::ShaderConfig shader_config = CreateShaderConfig();
    render::SkyboxConfig skybox_config = CreateSkyboxConfig();
    render::RenderConfig render_con = {camera_con, light_con, shader_config};

    GLFWwindow* win = utility::InitGraphics(win_con);
    adapter::Adapter adapter(win, render_con, skybox_config);

    const analyzer::TriangleArr& tr_arr = analyzer.GetTriangles();
    const std::vector<bool>& intersect_status = analyzer.GetInterStatuses();
    for (size_t ind = 0; ind < triangles_number; ++ind) {
      adapter.ConvertTriangle(tr_arr[ind], intersect_status[ind]);
    }

    adapter.Draw();
  }

  utility::CleanResources();
}
