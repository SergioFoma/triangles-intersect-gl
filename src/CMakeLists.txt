add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/graphics)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/triangles)

set(TARGET triangle_intersect)
add_executable(${TARGET})
target_sources(${TARGET} PRIVATE main.cpp adapter.cpp read_data.cpp)
target_include_directories(${TARGET} PRIVATE ${INCLUDE})
target_link_libraries(${TARGET} PRIVATE graphics triangles CLI11::CLI11)
