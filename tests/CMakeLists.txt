include(GoogleTest)
set(TEST_TARGET_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/triangle_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/inter_analyzer_test.cpp
)
set(END_TO_END_SOURCES
  ${CMAKE_SOURCE_DIR}/src/read_data.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/end_to_end.cpp
)

set(BENCHMARK_SOURCES
  ${CMAKE_SOURCE_DIR}/src/read_data.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark.cpp
)

set(TEST_TARGET triangles_tests)
add_executable(${TEST_TARGET})
target_sources(${TEST_TARGET} PRIVATE ${TEST_TARGET_SOURCES})
target_include_directories(${TEST_TARGET} PRIVATE ${INCLUDE})
target_link_libraries(${TEST_TARGET} PRIVATE GTest::gtest_main triangles)

gtest_discover_tests(${TEST_TARGET})

set(END_TO_END_TARGET end_to_end_testing)
add_executable(${END_TO_END_TARGET})
target_sources(${END_TO_END_TARGET} PRIVATE ${END_TO_END_SOURCES})
target_include_directories(${END_TO_END_TARGET} PRIVATE ${INCLUDE})
target_link_libraries(${END_TO_END_TARGET} PRIVATE triangles)

set(BENCHMARK_TARGET benchmark_test)
add_executable(${BENCHMARK_TARGET})
target_sources(${BENCHMARK_TARGET} PRIVATE ${BENCHMARK_SOURCES})
target_include_directories(${BENCHMARK_TARGET} PRIVATE ${INCLUDE})
target_link_libraries(${BENCHMARK_TARGET} PRIVATE triangles benchmark::benchmark)
