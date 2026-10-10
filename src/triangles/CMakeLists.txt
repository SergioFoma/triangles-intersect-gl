set(TRIANGLES_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/triangle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inter_analyzer.cpp
)
add_library(triangles STATIC ${TRIANGLES_SRC})
target_link_libraries(triangles PUBLIC TBB::tbb)
target_include_directories(triangles PUBLIC ${INCLUDE} ${INCLUDE}/triangles)
