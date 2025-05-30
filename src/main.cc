#include <cmath>
#include <iostream>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Mesh.h"
#include "ShaderProgram.h"

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

Mesh generateUnitCube(void) {
  struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
  };

  std::vector<Vertex> vertices = {
    { { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },
    { {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },
    { {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },
    { {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },
    { { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },
    { { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },

    { { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },
    { {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },
    { {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },
    { {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },
    { { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },
    { { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },

    { { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f } },
    { { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f } },
    { { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f } },
    { { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f } },
    { { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f } },

    { {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f } },
    { {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f } },
    { {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f } },
    { {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f } },
    { {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f } },
    { {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f } },

    { { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f } },
    { {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f } },
    { {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f } },
    { {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f } },
    { { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f } },
    { { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f } },

    { { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f } },
    { {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f } },
    { {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f } },
    { {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f } },
    { { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f } },
    { { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f } },
  };

  return Mesh(vertices);
}

Mesh generateUnitSphere(int sectors = 36, int stacks = 18) {
  struct Vertex {
    glm::vec3 position;
  };

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  const float PI = 3.1415926f;
  float sectorStep = 2 * PI / sectors;
  float stackStep = PI / stacks;

  for (int i = 0; i <= stacks; ++i) {
    float stackAngle = PI / 2 - i * stackStep;
    float xz = std::cos(stackAngle);
    float y = std::sin(stackAngle);

    for (int j = 0; j <= sectors; ++j) {
      float sectorAngle = j * sectorStep;
      float x = xz * std::cos(sectorAngle);
      float z = xz * std::sin(sectorAngle);

      vertices.push_back(Vertex{ glm::vec3(x, y, z) });
    }
  }

  for (int i = 0; i < stacks; ++i) {
    int k1 = i * (sectors + 1);
    int k2 = k1 + sectors + 1;

    for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
      if (i != 0) {
        indices.push_back(k1);
        indices.push_back(k2);
        indices.push_back(k1 + 1);
      }

      if (i != stacks - 1) {
        indices.push_back(k1 + 1);
        indices.push_back(k2);
        indices.push_back(k2 + 1);
      }
    }
  }

  return Mesh(vertices, indices);
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

  glEnable(GL_DEPTH_TEST);

  printSystemInfo();

  std::unique_ptr<ShaderProgram> cubeShader = ShaderProgram::create(
    "assets/shaders/cubeShader.vs", "assets/shaders/cubeShader.fs");
  std::unique_ptr<ShaderProgram> lightShader = ShaderProgram::create(
    "assets/shaders/lightShader.vs", "assets/shaders/lightShader.fs");

  if (!cubeShader || !lightShader) {
    glfwTerminate();
    return -1;
  }

  Mesh cube = generateUnitCube();

  Mesh light = generateUnitSphere();
  glm::vec3 lightPosition(1.2f, 1.0f, 2.0f);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 projectionMatrix = camera.getProjectionMatrix(
      static_cast<float>(windowWidth) / windowHeight, 0.1f, 100.0f);
    glm::mat4 viewMatrix = camera.getViewMatrix();

    cubeShader->use();
    cubeShader->uniform("viewPos", camera.getPosition());
    /* Light properties */
    glm::vec3 lightColor{
      std::sin(currentFrame) * 2.0f,
      std::sin(currentFrame) * 0.7f,
      std::sin(currentFrame) * 1.3f
    };
    glm::vec3 lightDiffuseColor = lightColor * 0.5f;
    glm::vec3 lightAmbientColor = lightDiffuseColor * 0.2f;
    cubeShader->uniform("light.position", lightPosition);
    cubeShader->uniform("light.ambient", lightAmbientColor);
    cubeShader->uniform("light.diffuse", lightDiffuseColor);
    cubeShader->uniform("light.specular", 1.0f, 1.0f, 1.0f);
    /* Material properties */
    cubeShader->uniform("material.ambient", 1.0f, 0.5f, 0.31f);
    cubeShader->uniform("material.diffuse", 1.0f, 0.5f, 0.31f);
    cubeShader->uniform("material.specular", 0.5f, 0.5f, 0.5f);
    cubeShader->uniform("material.shininess", 32.0f);
    /* Transformations */
    glm::mat4 cubeModelMatrix = glm::mat4(1.0f);
    glm::mat3 cubeNormalMatrix = glm::mat3(glm::transpose(glm::inverse(cubeModelMatrix)));
    cubeShader->uniform("normalMatrix", cubeNormalMatrix);
    cubeShader->uniform("modelMatrix", cubeModelMatrix);
    cubeShader->uniform("viewMatrix", viewMatrix);
    cubeShader->uniform("projectionMatrix", projectionMatrix);
    /* Render the cube. */
    cube.draw();

    lightShader->use();
    lightShader->uniform("lightColor", lightColor);
    glm::mat4 lightModelMatrix = glm::mat4(1.0f);
    lightModelMatrix = glm::translate(lightModelMatrix, lightPosition);
    lightModelMatrix = glm::scale(lightModelMatrix, glm::vec3(0.1f));
    lightShader->uniform("modelMatrix", lightModelMatrix);
    lightShader->uniform("viewMatrix", viewMatrix);
    lightShader->uniform("projectionMatrix", projectionMatrix);
    light.draw();

    glfwSwapBuffers(window);
    /* The `glfwPollEvents` function checks if any events are triggered,
    * updates the window state, and calls the corresponding functions (which
    * we can register via callback methods). */
    glfwPollEvents();
  }

  glfwTerminate();

  return 0;
}
