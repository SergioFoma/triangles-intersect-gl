# === triangles ===

set(TRIANGLES_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/triangles.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/triangle_handler.cpp
)

target_sources(${TRIANGLES} PRIVATE ${TRIANGLES_SRC})
target_include_directories(${TRIANGLES} PRIVATE ${PROJECT_SOURCE_DIR}/include/triangles)
