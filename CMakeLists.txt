cmake_minimum_required(VERSION 3.24)
project(Voxis_CPP VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Dawn REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.h")
add_executable(bin ${SOURCES})

target_include_directories(bin PRIVATE
    ${CMAKE_SOURCE_DIR}/dawn/include
)

target_link_libraries(bin PRIVATE
    ${CMAKE_SOURCE_DIR}/dawn/out/Release/src/dawn/glfw/libdawn_glfw.a
    dawn::webgpu_dawn
    glfw
)

target_compile_options(bin PRIVATE -Wall -Wextra -pedantic)