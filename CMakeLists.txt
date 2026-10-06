cmake_minimum_required(VERSION 3.14)

option(BUILD_TESTING "Build test" OFF)

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

# === project build ===

project(triangles_gl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL
    "Generate compile_commands.json" FORCE)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O2 -Wall")
set(SRC ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE ${PROJECT_SOURCE_DIR}/include)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# === packages ===

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
fetch_if_not_available(TBB https://github.com/uxlfoundation/oneTBB/archive/refs/heads/master.zip)

# === add_executables ===

add_subdirectory(${SRC})

# === tests ===

if (${BUILD_TESTING})
    fetch_if_not_available(GTest https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip)
    enable_testing()
    add_subdirectory(tests)
endif()

