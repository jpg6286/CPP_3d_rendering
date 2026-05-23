>>>>>>> db06b04a293e6b61ac1ff1186624dfb10c62f19b
cmake_minimum_required(VERSION 3.10)
project(OpenGl_Project
VERSION 0.1
DESCRIPTION "Physics")
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
find_path(GLUT_INCLUDE_DIR NAMES GL/glut.h PATHS /usr/include)
find_library(GLUT_LIBRARY glut PATHS /usr/lib)
find_package(OpenGL REQUIRED)
add_executable(${PROJECT_NAME} main.cpp physics_engine.cpp OpenGL_render.cpp)
configure_file(config.h.in config.h)
target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_BINARY_DIR}" ${GLUT_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${GLUT_LIBRARY} OpenGL::GL OpenGL::GLU)
