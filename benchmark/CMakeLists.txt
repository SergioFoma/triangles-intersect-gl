set(BENCHMARK_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark.cpp
)

set(BENCHMARK_TARGET benchmark_test)
add_executable(${BENCHMARK_TARGET})
target_sources(${BENCHMARK_TARGET} PRIVATE ${BENCHMARK_SOURCES})
target_include_directories(${BENCHMARK_TARGET} PRIVATE ${INCLUDE})
target_link_libraries(${BENCHMARK_TARGET} PRIVATE triangles benchmark::benchmark)
