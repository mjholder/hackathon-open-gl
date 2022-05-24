cmake_minimum_required (VERSION 3.22)

project (hackathon-open-gl)

set (source_dir "${PROJECT_SOURCE_DIR}/src/")

file (GLOB source_files "${source_dir}/*.cpp")

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable (hackathon-open-gl ${source_files})

target_link_libraries(hackathon-open-gl glfw)
target_link_libraries(hackathon-open-gl OpenGL::GL)
