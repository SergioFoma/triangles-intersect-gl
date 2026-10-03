cmake_minimum_required(VERSION 3.14)

# === functions ===

function(fetch_if_not_available PKG DOWNLOAD_URL)
    find_package(${PKG} QUIET)
    if(NOT ${PKG}_FOUND)
        include(FetchContent)
        FetchContent_Declare(${PKG}_fetchcontent
            URL ${DOWNLOAD_URL}
        )
        FetchContent_MakeAvailable(${PKG}_fetchcontent)
    endif()
endfunction()

# === project build ====

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

fetch_if_not_available(GTest https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip)
fetch_if_not_available(TBB https://github.com/uxlfoundation/oneTBB/archive/refs/heads/master.zip)

# === triangles ===

#set(TRIANGLES triangles)
#add_library(${TRIANGLES} STATIC)
#target_link_libraries(${TRIANGLES} PUBLIC TBB::tbb)
#add_subdirectory(${SRC}/${TRIANGLES})
#add_subdirectory(tests)

# === adapter ===
set(ADAPTER adapter)
add_subdirectory(${SRC}/${ADAPTER})
