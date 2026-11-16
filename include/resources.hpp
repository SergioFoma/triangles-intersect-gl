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

class Resource {
 public:
  static std::string GetDefaultPath(const std::string& bin_path) {

    std::filesystem::path root = paths::GetAbsRootPath(bin_path);
    std::filesystem::path def = root / "resource";

    std::string def_str = def.string();
    return def_str;
  }

  Resource(const std::string& resource_paths, const TextureMap& texture_map)
      : texture_sides_(kTextureNumber) {
    std::filesystem::path abs_path = paths::GetAbsPath(resource_paths);
    for (const auto& file : std::filesystem::directory_iterator(abs_path)) {
      const auto& filepath = file.path();
      const auto& filename = file.path().filename();
      if (texture_map.find(filename.string()) != texture_map.end()) {
        texture_sides_[texture_map.at(filename.string())] = file.path().string();
        continue;
      }

      std::string filename_str = filename.string();
      std::string filepath_str = filepath.string();
      if (filename_str == "triangle.vert")
        triangle_vert_ = filepath_str;
      else if (filename_str == "triangle.frag")
        triangle_frag_ = filepath_str;
      else if (filename_str == "skybox.vert")
        skybox_vert_ = filepath_str;
      else if (filename_str == "skybox.frag")
        skybox_frag_ = filepath_str;
      else if (filename_str == "point.vert")
        point_vert_ = filepath_str;
      else if (filename_str == "point.frag")
        point_frag_ = filepath_str;
      else if (filename_str == "line.vert")
        line_vert_ = filepath_str;
      else if (filename_str == "line.frag")
        line_frag_ = filepath_str;
      else {
        prog_error::ErrorInfo inf = {prog_error::ErrorCode::kResource,
                                     "Unknown file!", "Resource::Resource"};
        throw prog_error::ProgError(std::move(inf));
      }
    }

    if (texture_sides_.size() != kTextureNumber) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kResource,
                                   "Files for texture havn't been found!",
                                   "Resource::Resource"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  const std::vector<std::string>& GetTextureSides() const {
    return texture_sides_;
  }
  const std::string& GetTriangleVert() const {
    return triangle_vert_;
  }
  const std::string& GetTriangleFrag() const {
    return triangle_frag_;
  }
  const std::string& GetSkyboxVert() const {
    return skybox_vert_;
  }
  const std::string& GetSkyboxFrag() const {
    return skybox_frag_;
  }
  const std::string& GetPointVert() const {
    return point_vert_;
  }
  const std::string& GetPointFrag() const {
    return point_frag_;
  }
  const std::string& GetLineVert() const {
    return line_vert_;
  }
  const std::string& GetLineFrag() const {
    return line_frag_;
  }

 private:
  std::vector<std::string> texture_sides_;
  std::string triangle_vert_;
  std::string triangle_frag_;
  std::string skybox_vert_;
  std::string skybox_frag_;
  std::string point_vert_;
  std::string point_frag_;
  std::string line_vert_;
  std::string line_frag_;
};
} // namespace resource

#endif
