cmake_minimum_required(VERSION 3.11)

project(graphics)
SET(TARGET graphics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O2 -Wall")

set(SRC src)
set(INCLUDE include)
set(GRAPHICS graphics)
set(TRIANGLES triangles)

# find OpenGL packets
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(${TARGET}
    ${SRC}/${GRAPHICS}/main.cpp
    ${SRC}/${GRAPHICS}/utility.cpp
    ${SRC}/${GRAPHICS}/triangles_rendering.cpp
    ${SRC}/${GRAPHICS}/parsing_shaders.cpp
    ${SRC}/${GRAPHICS}/glad.c
)

target_include_directories(${TARGET} PUBLIC
    ${INCLUDE}
    ${INCLUDE}/${GRAPHICS}
    ${INCLUDE}/${TRIANGLES}
)

target_link_libraries(${TARGET} PUBLIC
    OpenGL::GL
    glfw
    ${CMAKE_DL_LIBS} # for GLAD
)
