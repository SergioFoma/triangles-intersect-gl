cmake_minimum_required(VERSION 3.14)

project(triangles_gl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL
    "Generate compile_commands.json" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O2 -Wall")

set(SRC ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE ${PROJECT_SOURCE_DIR}/include)

# === triangles-intersect-gl ===
set(TARGET triangles_gl)

# === graphics ===
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
set(GRAPHICS graphics)
add_executable(${GRAPHICS})
add_subdirectory(${SRC}/${GRAPHICS})

target_link_libraries(${GRAPHICS} PUBLIC
    OpenGL::GL
    glfw
    ${CMAKE_DL_LIBS} # for GLAD
)

# === tests ===


enable_testing()
include(FetchContent)
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
FetchContent_MakeAvailable(googletest)

set(TRIANGLES triangles)
add_library(${TRIANGLES} STATIC)
add_subdirectory(${SRC}/triangles)
add_subdirectory(tests)
