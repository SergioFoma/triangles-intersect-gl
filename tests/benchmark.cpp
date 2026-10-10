#include "benchmark/benchmark.h"
#include <array>
#include <string>

#include "triangles/inter_analyzer.hpp"
#include "read_data.hpp"

namespace {

constexpr unsigned int kBenchmarkNumber = 5;

void BmTrianglesIntersect(benchmark::State& state, const std::string& file_name) {
  triangles::TriangleArr triangles = reader::ReadData(file_name);

  while (state.KeepRunning()) {
    triangles::InterAnalyzer analyzer(triangles);
    benchmark::DoNotOptimize(analyzer);
  }
}

} // namespace

int main(int argc, char** argv) {

  const char* bin_relative_path = argv[0];
  const std::string materials_str = reader::GetAbsMaterials(bin_relative_path);

  std::array<const std::string, kBenchmarkNumber> input_data = {
    materials_str + "/triangles_100.txt",
    materials_str + "/triangles_1000.txt",
    materials_str + "/triangles_10000.txt",
    materials_str + "/triangles_100000.txt",
    materials_str + "/triangles_1000000.txt"
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
