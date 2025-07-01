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

add_library(framework STATIC
  framework/src/Camera.cc
  framework/src/Framework.cc
  framework/src/Mesh.cc
  framework/src/Model.cc
  framework/src/ShaderProgram.cc
  framework/src/TextureLoader.cc
)
target_include_directories(framework
  PUBLIC
    ${boost_pfr_SOURCE_DIR}/include
    framework/include
  PRIVATE
    third_party/stb
)
target_link_libraries(framework
  PUBLIC
    glad
    glm::glm
  PRIVATE
    assimp::assimp
    glfw
    OpenGL::GL)

add_executable(cubemap-skybox
  apps/cubemap-skybox/App.cc
)
target_link_libraries(cubemap-skybox PRIVATE framework)
