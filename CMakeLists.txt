cmake_minimum_required(VERSION 3.11)

project(triangles_gl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL
    "Generate compile_commands.json" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O2 -Wall")

set(SRC src)
set(INCLUDE include)

# find OpenGL packets
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# === triangles-intersect-gl ===
set(TARGET triangles_gl)

# === triangles ===
#set(TRIANGLES triangles)
#add_executable(${TRIANGLES})
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${SRC}/${TRIANGLES})

# === graphics ===
set(GRAPHICS graphics)
add_executable(${GRAPHICS})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${SRC}/${GRAPHICS})

target_link_libraries(${GRAPHICS} PUBLIC
    OpenGL::GL
    glfw
    ${CMAKE_DL_LIBS} # for GLAD
)


