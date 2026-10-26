add_library(triangles INTERFACE)
target_link_libraries(triangles INTERFACE TBB::tbb)
target_include_directories(triangles INTERFACE ${INCLUDE} ${INCLUDE}/triangles)
