cmake_minimum_required(VERSION 3.15)

project(learn-opengl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

file(GLOB GLAD_SOURCES ${PROJECT_SOURCE_DIR}/third_party/glad/src/glad.c)

add_executable(opengl_app
  ${GLAD_SOURCES}
  ${PROJECT_SOURCE_DIR}/src/main.cc
)
target_include_directories(opengl_app PRIVATE
  ${PROJECT_SOURCE_DIR}/third_party/glad/include
)
target_link_libraries(opengl_app PRIVATE glfw OpenGL::GL)
