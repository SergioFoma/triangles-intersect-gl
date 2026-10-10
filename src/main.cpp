#include <CLI/CLI.hpp>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <unordered_map>

#include "adapter.hpp"
#include "utility.hpp"
#include "read_data.hpp"
#include "prog_error.hpp"

namespace {

constexpr size_t kTextureNumber = 6;

namespace texture_id {
enum Id : unsigned int {
  kRight  = 0,
  kLeft   = 1,
  kTop    = 2,
  kBot    = 3,
  kBack   = 4,
  kFront  = 5
};
} // namespace texture_id

const std::unordered_map<std::string, unsigned int> texture_map = {
  {"bkg1_right.png" , texture_id::kRight},
  {"bkg1_left.png"  , texture_id::kLeft},
  {"bkg1_top.png"   , texture_id::kTop},
  {"bkg1_bot.png"   , texture_id::kBot},
  {"bkg1_back.png"  , texture_id::kBack},
  {"bkg1_front.png" , texture_id::kFront}
};

std::filesystem::path GetAbsPath(const std::string& resource_paths) {

  std::filesystem::path resources(resource_paths);
   if (!std::filesystem::exists(resources)) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kMain,
                                   "Error of finding resource folder",
                                   "GetAbsPath"};
    throw prog_error::ProgError(std::move(inf));
  }

  std::filesystem::path current_wrc = std::filesystem::current_path();
  std::filesystem::path abs_path = std::filesystem::relative(resources, current_wrc);

  return abs_path;
}

struct Resource {
  std::vector<std::string> texture_sides;
  std::string triangle_vert;
  std::string triangle_frag;
  std::string skybox_vert;
  std::string skybox_frag;

  static std::string GetDefaultPath(const std::string& bin_path) {
    // /..triangles-intersect-gl/Build/bin/Triangle_intersect
    // triangles-intersect-gl/Build/bin/triangle_intersect --> Build/bin
    // triangles-intersect-gl/Build/bin --> Build
    // triangles-intersect-gl/Build --> triangles-intersect-gl

    std::filesystem::path bin = GetAbsPath(bin_path);
    std::filesystem::path root = bin.parent_path().parent_path().parent_path();
    std::filesystem::path def = root / "resource";

    std::string def_str = def.string();
    return def_str;
  }

  Resource(const std::string& resource_paths): texture_sides(kTextureNumber) {
  std::filesystem::path abs_path = GetAbsPath(resource_paths);
  for (const auto& file: std::filesystem::directory_iterator(abs_path)) {
    const auto& filepath = file.path();
    const auto& filename = file.path().filename();
    if (texture_map.find(filename) != texture_map.end()) {
      texture_sides[texture_map.at(filename.string())] = file.path().string();
      continue;
    }

    std::string filename_str = filename.string();
    std::string filepath_str = filepath.string();
    if (filename_str == "triangle.vert")      triangle_vert = filepath_str;
    else if (filename_str == "triangle.frag") triangle_frag = filepath_str;
    else if (filename_str == "skybox.vert")   skybox_vert   = filepath_str;
    else if (filename_str == "skybox.frag")   skybox_frag   = filepath_str;
    else {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kMain,
                                  "Unknown file!",
                                  "Resource::Resource"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

    if (texture_sides.size() != kTextureNumber) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kMain,
                                   "Files for texture havn't been found!",
                                   "Resource::Resource"};
      throw prog_error::ProgError(std::move(inf));
    }
  }
};

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

render::ShaderConfig CreateShaderConfig(const Resource& resource) {
  int position_loc = 0;               // location in shaders
  int color_loc = 1;                  // location in shaders
  int normal_loc = 2;                 // location in shaders

  render::ShaderConfig shader_con = {resource.triangle_vert, resource.triangle_frag,
                                  position_loc, color_loc, normal_loc};
  return shader_con;
}

render::SkyboxConfig CreateSkyboxConfig(const Resource& resource) {

  unsigned int sides_number = 6;
  unsigned int triangles_number = 36;

  render::ShaderConfig shader_con = {resource.skybox_vert, resource.skybox_frag,
                                      0, 1, 2};

  render::SkyboxConfig skybox_con = {resource.texture_sides, sides_number,
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

adapter::Adapter InitAdapter(const std::string& resource_paths, const char* bin_paths) {
  std::string target_paths;

  if (resource_paths.empty()) {
    target_paths = Resource::GetDefaultPath(bin_paths);
  } else {
    target_paths = resource_paths;
  }
  Resource resource(target_paths);

  utility::WinConfig win_con = CreateWinConfig();
  render::LightConfig light_con = CreateLightConfig();
  render::CameraConfig camera_con = CreateCameraConfig(win_con);
  render::ShaderConfig shader_config = CreateShaderConfig(resource);
  render::SkyboxConfig skybox_config = CreateSkyboxConfig(resource);
  render::RenderConfig render_con = {camera_con, light_con, shader_config};

  GLFWwindow* win = utility::InitGraphics(win_con);
  adapter::Adapter adapter(win, render_con, skybox_config);

  return adapter;
}
} // namespace

int main(int argc, char** argv) {

  try {
    CLI::App app("Finding triangles intersection and rendering them");
    argv = app.ensure_utf8(argv);
    std::string input_path;
    std::string resource_paths;
    bool is_only_intersect = false;
    app.add_option("-f,--input_file,input_name", input_path, "The input data file")->required();
    app.add_option("-r,--resource", resource_paths, "The path to resource");
    app.add_flag("--only-intersection-analysis", is_only_intersect, "Analyzes only intersection");
    CLI11_PARSE(app, argc, argv);

    triangles::TriangleArr triangles = reader::ReadData(input_path);
    triangles::InterAnalyzer analyzer(triangles);

    if (!is_only_intersect) {
      adapter::Adapter adapter = InitAdapter(resource_paths, argv[0]);

      size_t triangles_number = triangles.size();
      for (size_t ind = 0; ind < triangles_number; ++ind) {
        adapter.ConvertTriangle(triangles[ind], analyzer.DoesIntersect(ind));
      }

      adapter.Draw();
    }
  } catch (const prog_error::ProgError& e) {
    const prog_error::ErrorInfo& info = e.GetErrorInfo();
    std::cerr << "Error in " << info.func_name << "\nLog: " << e.what() << '\n';
    utility::CleanResources();
    return static_cast<int>(info.code);
  }
  utility::CleanResources();
}
