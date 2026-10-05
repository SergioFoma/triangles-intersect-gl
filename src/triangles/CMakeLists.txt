set(TRIANGLES_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/triangle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inter_analyzer.cpp
)
add_library(triangles STATIC ${TRIANGLES_SRC})
target_include_directories(triangles PRIVATE ${INCLUDE}/triangles)
