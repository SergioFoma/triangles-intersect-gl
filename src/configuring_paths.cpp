
#include "configuring_paths.hpp"
#include "prog_error.hpp"

std::filesystem::path paths::GetAbsPath(const std::string& resource_paths) {

  std::filesystem::path resources(resource_paths);
  std::filesystem::path abs_path = std::filesystem::weakly_canonical(
                                        std::filesystem::absolute(resources));

  if (!std::filesystem::exists(resources)) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kMain,
                                  "Error of finding resource folder",
                                  "GetAbsPath"};
    throw prog_error::ProgError(std::move(inf));
  }

  return abs_path;
}

std::filesystem::path paths::GetAbsRootPath(const std::string& relative_bin_paths) {
  // .../root/Build/bin/triangle_intersect -->  .../root/Build/bin/
  // .../root/Build/bin/ --> .../root/Build
  // .../root/Build --> .../root/
  std::filesystem::path bin_path = GetAbsPath(relative_bin_paths);
  std::filesystem::path root_path = bin_path.parent_path().parent_path().parent_path();

  return root_path;
}

std::string paths::GetAbsMaterials(const std::string& relative_bin_path) {

  std::filesystem::path root_path = GetAbsRootPath(relative_bin_path);
  std::filesystem::path materials = root_path / "materials";
  std::string materials_str = materials.string();

  return materials_str;
}
