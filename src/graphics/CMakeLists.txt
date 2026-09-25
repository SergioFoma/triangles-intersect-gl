set(GRAPHICS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utility.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/triangles_rendering.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/glad.c
		${CMAKE_CURRENT_SOURCE_DIR}/shader.cpp
)

target_sources(${GRAPHICS} PRIVATE ${GRAPHICS_SRC})

target_include_directories(${GRAPHICS} PUBLIC
    ../../${INCLUDE}
    ../../${INCLUDE}/${GRAPHICS}
)
