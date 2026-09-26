set(TEST_TARGET_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/point_test.cpp
                        ${CMAKE_CURRENT_SOURCE_DIR}/triangle_test.cpp)
include(GoogleTest)

set(TEST_TARGET triangles_tests)
add_executable(${TEST_TARGET})
target_sources(${TEST_TARGET} PRIVATE ${TEST_TARGET_SOURCES})
target_include_directories(${TEST_TARGET} PRIVATE ${INCLUDE})
target_link_libraries(${TEST_TARGET} PRIVATE GTest::gtest_main)
gtest_discover_tests(${TEST_TARGET})
