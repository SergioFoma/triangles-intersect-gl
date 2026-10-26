#include <CLI/CLI.hpp>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "adapter.hpp"
#include "prog_error.hpp"
#include "read_data.hpp"

namespace {

constexpr size_t kTextureNumber = 6;

using TextureMap = std::unordered_map<std::string, unsigned int>;

namespace texture_id {
enum Id : unsigned int {
  kRight = 0,
  kLeft = 1,
  kTop = 2,
  kBot = 3,
  kBack = 4,
  kFront = 5
};
}  // namespace texture_id

struct Resource {
  std::vector<std::string> texture_sides;
  std::string triangle_vert;
  std::string triangle_frag;
  std::string skybox_vert;
  std::string skybox_frag;
  std::string point_vert;
  std::string point_frag;
  std::string line_vert;
  std::string line_frag;

  static std::string GetDefaultPath(const std::string& bin_path) {

    std::filesystem::path root = reader::GetAbsRootPath(bin_path);
    std::filesystem::path def = root / "resource";

    std::string def_str = def.string();
    return def_str;
  }

  Resource(const std::string& resource_paths, const TextureMap& texture_map)
      : texture_sides(kTextureNumber) {
    std::filesystem::path abs_path = reader::GetAbsPath(resource_paths);
    for (const auto& file : std::filesystem::directory_iterator(abs_path)) {
      const auto& filepath = file.path();
      const auto& filename = file.path().filename();
      if (texture_map.find(filename.string()) != texture_map.end()) {
        texture_sides[texture_map.at(filename.string())] = file.path().string();
        continue;
      }

      std::string filename_str = filename.string();
      std::string filepath_str = filepath.string();
      if (filename_str == "triangle.vert")
        triangle_vert = filepath_str;
      else if (filename_str == "triangle.frag")
        triangle_frag = filepath_str;
      else if (filename_str == "skybox.vert")
        skybox_vert = filepath_str;
      else if (filename_str == "skybox.frag")
        skybox_frag = filepath_str;
      else if (filename_str == "point.vert")
        point_vert = filepath_str;
      else if (filename_str == "point.frag")
        point_frag = filepath_str;
      else if (filename_str == "line.vert")
        line_vert = filepath_str;
      else if (filename_str == "line.frag")
        line_frag = filepath_str;
      else {
        prog_error::ErrorInfo inf = {prog_error::ErrorCode::kMain,
                                     "Unknown file!", "Resource::Resource"};
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

  render::LightConfig light = {color,    cut_off, outer_cut_off,
                               constant, linear,  quadratic};

  return light;
}

render::CameraConfig CreateCameraConfig(const utility::WinConfig& win_con) {
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

  render::CameraConfig camera = {pos,        front,     up,
                                 speed,      aspect,    fovy,
                                 near_plane, far_plane, mouse_sensitivity};

  return camera;
}

render::ShaderConfig CreateShaderConfig(const Resource& resource,
                                        const std::string& vert_sh,
                                        const std::string& frag_sh) {
  int position_loc = 0;  // location in shaders
  int color_loc = 1;     // location in shaders
  int normal_loc = 2;    // location in shaders

  render::ShaderConfig shader_con = {vert_sh, frag_sh, position_loc, color_loc,
                                     normal_loc};
  return shader_con;
}

render::SkyboxConfig CreateSkyboxConfig(const Resource& resource) {

  unsigned int sides_number = 6;
  unsigned int triangles_number = 36;

  render::ShaderConfig shader_con = {resource.skybox_vert, resource.skybox_frag,
                                     0, 1, 2};

  render::SkyboxConfig skybox_con = {resource.texture_sides, sides_number,
                                     triangles_number, shader_con};

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

const TextureMap& CreateTextureMap() {
  static const TextureMap texture_map = {
      {"bkg1_right.png", texture_id::kRight},
      {"bkg1_left.png", texture_id::kLeft},
      {"bkg1_top.png", texture_id::kTop},
      {"bkg1_bot.png", texture_id::kBot},
      {"bkg1_back.png", texture_id::kBack},
      {"bkg1_front.png", texture_id::kFront}};

  return texture_map;
}

template <typename T>
requires std::is_floating_point_v<T> adapter::Adapter<T> InitAdapter(
    const std::string& resource_paths, const char* bin_paths,
    triangles::TriangleArray<T>& triangles) {
  std::string target_paths;

  if (resource_paths.empty()) {
    target_paths = Resource::GetDefaultPath(bin_paths);
  } else {
    target_paths = resource_paths;
  }

  const TextureMap& texture_map = CreateTextureMap();
  Resource resource(target_paths, texture_map);

  utility::WinConfig win_con = CreateWinConfig();
  render::LightConfig light_con = CreateLightConfig();
  render::CameraConfig camera_con = CreateCameraConfig(win_con);
  render::ShaderConfig tr_shader_config = CreateShaderConfig(
      resource, resource.triangle_vert, resource.triangle_frag);
  render::ShaderConfig point_shader_config =
      CreateShaderConfig(resource, resource.point_vert, resource.point_frag);
  render::ShaderConfig line_shader_config =
      CreateShaderConfig(resource, resource.line_vert, resource.line_frag);
  render::SkyboxConfig skybox_config = CreateSkyboxConfig(resource);
  render::RenderConfig render_con = {camera_con, light_con, tr_shader_config,
                                     point_shader_config, line_shader_config};

  utility::Window win = utility::InitGraphics(win_con);
  adapter::Adapter<T> adapter(std::move(win), render_con, skybox_config);

  return adapter;
}
}  // namespace

using calc_type = float;
int main(int argc, char** argv) {
  try {
    CLI::App app("Finding triangles intersection and rendering them");
    argv = app.ensure_utf8(argv);
    std::string input_path;
    std::string resource_paths;
    bool is_only_intersect = false;
    app.add_option("-f,--input_file,input_name", input_path,
                   "The input data file")
        ->required();
    app.add_option("-r,--resource", resource_paths, "The path to resource");
    app.add_flag("--only-intersection-analysis", is_only_intersect,
                 "Analyzes only intersection");
    CLI11_PARSE(app, argc, argv);

    triangles::TriangleArray<calc_type> triangles = reader::ReadData<calc_type>(input_path);
    adapter::Adapter<calc_type>::SortingByType(triangles);
    triangles::InterAnalyzer analyzer(triangles);

    if (!is_only_intersect) {
      adapter::Adapter adapter =
          InitAdapter<calc_type>(resource_paths, argv[0], triangles);

      size_t triangles_number = triangles.size();
      for (size_t ind = 0; ind < triangles_number; ++ind) {
        adapter.ConvertShape(triangles[ind], analyzer.DoesIntersect(ind));
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
