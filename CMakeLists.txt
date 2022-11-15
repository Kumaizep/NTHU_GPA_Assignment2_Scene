cmake_minimum_required(VERSION 3.24)

project(GPA2022_Assignment2)

aux_source_directory(src SRC_LIST)
aux_source_directory(include/glad GLAD_SRC_LIST)
add_executable(GPA2022_Assignment2 ${SRC_LIST} ${GLAD_SRC_LIST})

include_directories(include)
include_directories(include/glad)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/asset ${CMAKE_CURRENT_BINARY_DIR}/asset
)
add_dependencies(GPA2022_Assignment2 copy_assets)

set(CMAKE_CXX_FLAGS "-lGL -lGLEW -lglfw -lglut")

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_CXX_STANDARD 20)
