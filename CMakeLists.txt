cmake_minimum_required(VERSION 3.15)

project(learn-opengl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  set(CMAKE_VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

include(FetchContent)

FetchContent_Declare(
  boost_pfr
  GIT_REPOSITORY https://github.com/boostorg/pfr.git
  GIT_TAG boost-1.88.0
)
FetchContent_MakeAvailable(boost_pfr)

find_package(assimp REQUIRED)
find_package(glfw3 3.3 REQUIRED CONFIG)
find_package(glm 0.9.9 REQUIRED CONFIG)
find_package(OpenGL REQUIRED)

add_subdirectory(third_party/glad)

add_executable(opengl_app
  ${PROJECT_SOURCE_DIR}/src/Camera.cc
  ${PROJECT_SOURCE_DIR}/src/Camera.h
  ${PROJECT_SOURCE_DIR}/src/main.cc
  ${PROJECT_SOURCE_DIR}/src/Mesh.cc
  ${PROJECT_SOURCE_DIR}/src/Mesh.h
  ${PROJECT_SOURCE_DIR}/src/Model.cc
  ${PROJECT_SOURCE_DIR}/src/Model.h
  ${PROJECT_SOURCE_DIR}/src/ShaderProgram.cc
  ${PROJECT_SOURCE_DIR}/src/ShaderProgram.h
  ${PROJECT_SOURCE_DIR}/src/TextureLoader.cc
  ${PROJECT_SOURCE_DIR}/src/TextureLoader.h
)
target_include_directories(opengl_app PRIVATE
  ${boost_pfr_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/third_party/stb
)
target_link_libraries(opengl_app PRIVATE assimp::assimp glad glfw glm::glm OpenGL::GL)
