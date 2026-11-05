#ifndef RESOURCES_HPP_
#define RESOURCES_HPP_

#include <vector>
#include <string>
#include <unordered_map>

#include "configuring_paths.hpp"
#include "prog_error.hpp"

namespace resource {

using TextureMap = std::unordered_map<std::string, unsigned int>;

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
}  // namespace texture_id

inline const TextureMap& CreateTextureMap() {
  static const TextureMap texture_map = {
      {"bkg1_right.png" , texture_id::kRight},
      {"bkg1_left.png"  , texture_id::kLeft},
      {"bkg1_top.png"   , texture_id::kTop},
      {"bkg1_bot.png"   , texture_id::kBot},
      {"bkg1_back.png"  , texture_id::kBack},
      {"bkg1_front.png" , texture_id::kFront}};

  return texture_map;
}

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

    std::filesystem::path root = paths::GetAbsRootPath(bin_path);
    std::filesystem::path def = root / "resource";

    std::string def_str = def.string();
    return def_str;
  }

  Resource(const std::string& resource_paths, const TextureMap& texture_map)
      : texture_sides(kTextureNumber) {
    std::filesystem::path abs_path = paths::GetAbsPath(resource_paths);
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
} // namespace resource

#endif
