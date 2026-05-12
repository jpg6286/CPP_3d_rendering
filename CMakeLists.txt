cmake_minimum_required(VERSION 3.20)
project(Renderer LANGUAGES CXX)bi

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS )

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glad REQUIRED)
find_package(glm REQUIRED)

add_executable(renderer
    src/main.cpp
)

target_include_directories(renderer PRIVATE include)
target_link_libraries(renderer PRIVATE OpenGL::GL glfw glad::glad glm::glm)
