set(GRAPHICS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/triangles_rendering.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/glad.c
	${CMAKE_CURRENT_SOURCE_DIR}/shader.cpp
)
add_library(graphics STATIC ${GRAPHICS_SRC})
target_link_libraries(graphics PUBLIC OpenGL::GL
    glfw
    ${CMAKE_DL_LIBS})
target_include_directories(graphics PUBLIC ${INCLUDE}/graphics)
