#ifndef CONFIGURING_PATHS_HPP_
#define CONFIGURING_PATHS_HPP_

#include <filesystem>
#include <string>

namespace paths {

std::filesystem::path GetAbsPath(const std::string& resource_paths);

std::filesystem::path GetAbsRootPath(const std::string& relative_bin_paths);

std::string GetAbsMaterials(const std::string& relative_bin_path);

} // namespace paths

#endif
