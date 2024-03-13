cmake_minimum_required(VERSION 3.27)
project(OpenGL_SS)

set(CMAKE_CXX_STANDARD 11)

add_executable(OpenGL_SS main.cpp)
target_link_libraries(OpenGL_SS opengl32 glu32 freeglut)
