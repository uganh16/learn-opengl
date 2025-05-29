cmake_minimum_required(VERSION 3.15)

project(learn-opengl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  set(CMAKE_VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

find_package(glfw3 REQUIRED CONFIG)
find_package(OpenGL REQUIRED)

add_subdirectory(third_party/glad)

add_executable(opengl_app
  ${PROJECT_SOURCE_DIR}/src/main.cc
  ${PROJECT_SOURCE_DIR}/src/Mesh.cc
  ${PROJECT_SOURCE_DIR}/src/Mesh.h
  ${PROJECT_SOURCE_DIR}/src/ShaderProgram.cc
  ${PROJECT_SOURCE_DIR}/src/ShaderProgram.h
)
target_include_directories(opengl_app PRIVATE
  ${boost_pfr_SOURCE_DIR}/include
)
target_link_libraries(opengl_app PRIVATE glad glfw OpenGL::GL)
