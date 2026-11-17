#include <CLI/CLI.hpp>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "adapter.hpp"
#include "configs.hpp"
#include "graphics/glfw_session.hpp"
#include "graphics/graphics_container.hpp"
#include "pre_analysis.hpp"
#include "prog_error.hpp"
#include "reader.hpp"
#include "triangles/frame_analysis.hpp"
#include "triangles/pre_analysis.hpp"
#include "triangles/triangle.hpp"
#include "triangles/triangles_storage.hpp"
#include "triangles/rotation_updater.hpp"

#define INIT_CLI(argc, argv, input_path, resource_paths)                     \
  do {                                                                       \
    CLI::App app("Finding triangles intersection and rendering them");       \
    argv = app.ensure_utf8(argv);                                            \
    app.add_option("-f,--input_file,input_name", input_path,                 \
                   "The input data file")                                    \
        ->required();                                                        \
    app.add_option("-r,--resource", resource_paths, "The path to resource"); \
    CLI11_PARSE(app, argc, argv);                                            \
  } while (false)

using calc_type = double;

int main(int argc, char** argv) {
  try {
    std::string input_path;
    std::string resource_paths;
    INIT_CLI(argc, argv, input_path, resource_paths);

    const char* bin_paths = argv[0];
    config::UserConfigs user_configs{resource_paths, bin_paths};

    glfw::GlfwSession glfw_session{user_configs.GetWinCon()};

    utility::Window window = utility::InitWindow(user_configs.GetWinCon());

    reader::Reader<calc_type> obj_reader{input_path};

    triangles::TrianglesStorage<calc_type> tr_storage{obj_reader};

    video_buff::BufferManager buffer_manager{
        user_configs.GetTrShCon(),
        user_configs.GetSkyboxCon(),
    };

    graphics::GraphicsContainer graphics_container{
        tr_storage.GetShapesNumber(), user_configs, buffer_manager};

    triangles::RotationUpdater<calc_type> updater{glfwGetTime()};

    adapter::Adapter<calc_type> adapter{tr_storage};

    triangles::PreAnalyzer<calc_type> pre_analyzer;
    tr_storage.AcceptExternal(pre_analyzer);
    triangles::FrameAnalyzer<calc_type> frame_analyzer;

    while (glfwGetTime() < obj_reader.GetTotalTime() &&
           glfwGetKey(window.GetPtr(), GLFW_KEY_ESCAPE) != GLFW_PRESS) {

      tr_storage.AcceptExternal(frame_analyzer);

      const triangles::TrianglesArray<calc_type>& triangles =
          tr_storage.GetCurrentPos();

      adapter.ConvertShapes(triangles, tr_storage.GetPredicates(),
                            tr_storage.GetShapesNumber());
      adapter.Draw(window, buffer_manager, graphics_container, user_configs);

      tr_storage.AcceptExternal(updater);
    }
  } catch (const prog_error::ProgError& e) {
    const prog_error::ErrorInfo& info = e.GetErrorInfo();
    std::cerr << "Error in " << info.func_name << "\nLog: " << e.what() << '\n';
    return static_cast<int>(info.code);
  }
}
