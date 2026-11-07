#include <CLI/CLI.hpp>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "prog_error.hpp"
#include "triangles/triangle.hpp"
#include "triangles/triangles_storage.hpp"
#include "reader.hpp"
#include "graphics_container.hpp"
#include "configs.hpp"
#include "adapter.hpp"
#include "updater.hpp"
#include "triangles/inter_analyzer.hpp"

#define INIT_CLI(argc, argv, input_path, resource_paths)                      \
  do {                                                                        \
    CLI::App app("Finding triangles intersection and rendering them");        \
    argv = app.ensure_utf8(argv);                                             \
    bool is_only_intersect = false;                                           \
    app.add_option("-f,--input_file,input_name", input_path,                  \
                   "The input data file")->required();                        \
    app.add_option("-r,--resource", resource_paths, "The path to resource");  \
    app.add_flag("--only-intersection-analysis", is_only_intersect,           \
                 "Analyzes only intersection");                               \
    CLI11_PARSE(app, argc, argv);                                             \
  } while (false)

using calc_type = float;

int main(int argc, char** argv) {
  try {
    std::string input_path;
    std::string resource_paths;
    INIT_CLI(argc, argv, input_path, resource_paths);

    const char* bin_paths = argv[0];
    config::UserConfigs user_configs{resource_paths, bin_paths};

    utility::Window window = utility::InitGraphics(user_configs.win_con);

    reader::Reader<calc_type> obj_reader{input_path};

    storage::TrianglesStorage<calc_type> tr_storage{obj_reader};

    video_buff::BufferManager buffer_manager{user_configs.tr_shader_con,
                                             user_configs.skybox_con};

    graphics::GraphicsContainer graphics_container{tr_storage.GetShapesNumber(),
                                                   user_configs,
                                                   buffer_manager};

    updater::Updater<calc_type> updater{glfwGetTime()};

    adapter::Adapter<calc_type>::SortingByType(tr_storage.GetTriangles());

    adapter::Adapter<calc_type> adapter;

    while (glfwGetTime() < obj_reader.GetTotalTime() &&
           glfwGetKey(window.GetPtr(), GLFW_KEY_ESCAPE) != GLFW_PRESS) {

      triangles::TrianglesArray<calc_type>& triangles = tr_storage.GetTriangles();

      triangles::InterAnalyzer<calc_type> analyzer(triangles);

      adapter.Clear();

      size_t shapes_number = tr_storage.GetShapesNumber().shapes_number;
      for (size_t ind = 0; ind < shapes_number; ++ind) {
        adapter.ConvertShape(triangles[ind], analyzer.DoesIntersect(ind));
      }

      adapter.Draw(window, buffer_manager, graphics_container, user_configs);

      updater.Update(tr_storage);
    }
  } catch (const prog_error::ProgError& e) {
    const prog_error::ErrorInfo& info = e.GetErrorInfo();
    std::cerr << "Error in " << info.func_name << "\nLog: " << e.what() << '\n';
    utility::CleanResources();
    return static_cast<int>(info.code);
  }
  utility::CleanResources();
}
