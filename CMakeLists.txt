cmake_minimum_required(VERSION 3.10)
project(Renderer3D VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(POLICY CMP0072)
    cmake_policy(SET CMP0072 NEW)
endif()

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/physics_engine.cpp
    src/OpenGL_render.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include
    ${GLUT_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${GLUT_LIBRARIES}
    OpenGL::GL
    OpenGL::GLU
)

configure_file(config.h.in config.h)
