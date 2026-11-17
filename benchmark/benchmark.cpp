#include "benchmark/benchmark.h"
#include <array>
#include <string>

#include "reader.hpp"
#include "configuring_paths.hpp"

#include "triangles/pre_analysis.hpp"
#include "triangles/frame_analysis.hpp"
#include "triangles/rotation_updater.hpp"

using calc_type = double;
namespace {

constexpr unsigned int kBenchmarkNumber = 6;

void BmTrianglesIntersect(benchmark::State& state, const std::string& file_name) {
  reader::Reader<calc_type> obj_reader{file_name};

  triangles::TrianglesStorage<calc_type> tr_storage{obj_reader};

  triangles::PreAnalyzer<calc_type> pre_analyzer;
  tr_storage.AcceptExternal(pre_analyzer);

  triangles::FrameAnalyzer<calc_type> frame_analyzer;

  triangles::RotationUpdater<calc_type> updater{glfwGetTime()};

  while (state.KeepRunning()) {

    tr_storage.AcceptExternal(frame_analyzer);

    tr_storage.AcceptExternal(updater);
  }
}

} // namespace

int main(int argc, char** argv) {

  const char* bin_relative_path = argv[0];
  const std::string materials_str = paths::GetAbsMaterials(bin_relative_path);

  std::array<const std::string, kBenchmarkNumber> input_data = {
    materials_str + "/triangles_100.txt",
    materials_str + "/triangles_1000.txt",
    materials_str + "/triangles_10000.txt",
    materials_str + "/triangles_100000.txt",
    materials_str + "/triangles_1000000.txt",
    materials_str + "/triangles_point_line.txt"
  };

  for (const std::string& file_name: input_data) {
    benchmark::RegisterBenchmark(file_name.c_str(),
                                 [file_name](benchmark::State& state) {
                                  BmTrianglesIntersect(state, file_name);
                                 })->Unit(benchmark::kMillisecond);
  }

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
}
