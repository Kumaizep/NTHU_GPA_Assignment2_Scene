cmake_minimum_required(VERSION 3.24)

project(GPA2022_Assignment2)

aux_source_directory(src SRC_LIST)
aux_source_directory(include/glad GLAD_SRC_LIST)
aux_source_directory(include/imgui IMGUI_SRC_LIST)
set(IMGUI_SRC 
    include/imgui/backends/imgui_impl_glfw.cpp 
    include/imgui/backends/imgui_impl_opengl3.cpp
)
add_executable(GPA2022_Assignment2 ${SRC_LIST} ${GLAD_SRC_LIST} ${IMGUI_SRC_LIST} ${IMGUI_SRC})

include_directories(include)
include_directories(include/glad)
include_directories(include/imgui)
include_directories(include/imgui/backends)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/asset ${CMAKE_CURRENT_BINARY_DIR}/asset
)
add_dependencies(GPA2022_Assignment2 copy_assets)

set(CMAKE_CXX_FLAGS "-lGL -lGLEW -lglfw -lglut -lassimp")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 20)
