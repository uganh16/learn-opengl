#include <cmath>
#include <iostream>
#include <memory>

#include <assimp/version.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include "ShaderProgram.h"
#include "TextureLoader.h"

float lastFrame = 0.0f;
float deltaTime = 0.0f; /* Time between current frame and last frame. */

int windowWidth  = 800;
int windowHeight = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

/* Initialize the last mouse positions to be in the center of screen. */
float lastX;
float lastY;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  windowWidth = width;
  windowHeight = height;
  glViewport(0, 0, windowWidth, windowHeight);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  static bool bFirstMouse = true;

  if (bFirstMouse) {
    lastX = xpos;
    lastY = ypos;
    bFirstMouse = false;
  }

  float xoffset = static_cast<float>(xpos) - lastX;
  /* Reversed since y-coordinates range from buttom to top. */
  float yoffset = lastY - static_cast<float>(ypos);

  lastX = xpos;
  lastY = ypos;

  camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  camera.processMouseScroll(static_cast<float>(yoffset));
}

void printSystemInfo(void) {
  int glfwMajor, glfwMinor;
  glfwGetVersion(&glfwMajor, &glfwMinor, NULL);
  std::cout << "GLFW version: " << glfwMajor << "." << glfwMinor << std::endl;

  const GLchar* glVersion = (const GLchar*)glGetString(GL_VERSION);
  std::cout << "OpenGL version: " << (glVersion ? glVersion : "unknown") << std::endl;

  const GLchar* renderer = (const GLchar*)glGetString(GL_RENDERER);
  std::cout << "Renderer: " << (renderer ? renderer : "unknown") << std::endl;

  const GLchar* glslVersion = (const GLchar*)glGetString(GL_SHADING_LANGUAGE_VERSION);
  std::cout << "GLSL version: " << (glslVersion ? glslVersion : "unknown") << std::endl;

  GLint maxVertexAttribs;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
  std::cout << "Maximum number of vertex attributes supported: " << maxVertexAttribs << std::endl;

  std::cout << "Assimp version: " << aiGetVersionMajor() << "." << aiGetVersionMinor() << std::endl;
}

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  float cameraSpeed = 2.5f * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.processMovement(CameraMovement::FORWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.processMovement(CameraMovement::BACKWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.processMovement(CameraMovement::LEFT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.processMovement(CameraMovement::RIGHT, deltaTime);
  }
}

int main(void) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__) && defined(__MACH__)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Learn OpenGL", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  /* Tell GLFW to make the context of our window the main context on the
   * current thread. */
  glfwMakeContextCurrent(window);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  /**
   * Tell GLFW to capture the mouse.
   *
   * Capturing a cursor means that, once the application has focus, the mouse
   * stays within the center of the window (unless the application loses focus
   * or quits).
   */
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    return -1;
  }

  printSystemInfo();

  glEnable(GL_DEPTH_TEST);

  std::unique_ptr<ShaderProgram> shaderProgram = ShaderProgram::create(
    "assets/shaders/depthTestingShader.vs", "assets/shaders/depthTestingShader.fs");
  if (!shaderProgram) {
    glfwTerminate();
    return -1;
  }

  struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
  };

  Mesh cube(std::vector<Vertex>{
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f} },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f} },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f} },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f} },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f} },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f} },

    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f} },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f} },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f} },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f} },
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f} },
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f} },

    { { -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f} },
    { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f} },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f} },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f} },
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f} },
    { { -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f} },

    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f} },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f} },
    { {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f} },
    { {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f} },
    { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f} },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f} },

    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f} },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f} },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f} },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f} },
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f} },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f} },

    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f} },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f} },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f} },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f} },
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f} },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f} },
  });

  Mesh floor(std::vector<Vertex>{
    { {  5.0f, -0.5f,  5.0f }, { 2.0f, 0.0f } },
    { { -5.0f, -0.5f,  5.0f }, { 0.0f, 0.0f } },
    { { -5.0f, -0.5f, -5.0f }, { 0.0f, 2.0f } },

    { {  5.0f, -0.5f,  5.0f }, { 2.0f, 0.0f } },
    { { -5.0f, -0.5f, -5.0f }, { 0.0f, 2.0f } },
    { {  5.0f, -0.5f, -5.0f }, { 2.0f, 2.0f } },
  });

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaderProgram->use();

    glm::mat4 projectionMatrix = camera.getProjectionMatrix(
      static_cast<float>(windowWidth) / windowHeight, 0.1f, 100.0f);
    glm::mat4 viewMatrix = camera.getViewMatrix();
    shaderProgram->uniform("projectionMatrix", projectionMatrix);
    shaderProgram->uniform("viewMatrix", viewMatrix);

    glm::mat4 cube1ModelMatrix = glm::mat4(1.0f);
    cube1ModelMatrix = glm::translate(cube1ModelMatrix, glm::vec3(-1.0f, 0.0f, -1.0f));
    shaderProgram->uniform("modelMatrix", cube1ModelMatrix);
    cube.draw(*shaderProgram);

    glm::mat4 cube2ModelMatrix = glm::mat4(1.0f);
    cube2ModelMatrix = glm::translate(cube2ModelMatrix, glm::vec3(2.0f, 0.0f, 0.0f));
    shaderProgram->uniform("modelMatrix", cube2ModelMatrix);
    cube.draw(*shaderProgram);

    shaderProgram->uniform("modelMatrix", glm::mat4(1.0f));
    floor.draw(*shaderProgram);

    glfwSwapBuffers(window);
    /* The `glfwPollEvents` function checks if any events are triggered,
    * updates the window state, and calls the corresponding functions (which
    * we can register via callback methods). */
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
