cmake_minimum_required(VERSION 3.10)


find_package(OpenGL REQUIRED)


pybind11_add_module(context context.cpp "${PROJECT_SOURCE_DIR}/external/glfw/deps/glad/gl.h"
        "${PROJECT_SOURCE_DIR}/external/glfw/deps/glad_gl.c")

target_link_libraries(context PRIVATE glfw)

include_directories(" ${PROJECT_SOURCE_DIR}/external/glfw/deps")