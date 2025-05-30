#include <iostream>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

GLuint loadTexture(const std::string& path, bool flipVertically = true) {
  stbi_set_flip_vertically_on_load(flipVertically);

  /* Load and generate the texture. */
  int width, height, nrChannels;
  stbi_uc* image = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
  if (image == NULL) {
    std::cerr << "Failed to load image: " << path << " (" << stbi_failure_reason() << ")" << std::endl;
    return 0;
  }

  GLenum format;
  if (nrChannels == 1) {
    format = GL_RED;
  } else if (nrChannels == 3) {
    format = GL_RGB;
  } else if (nrChannels == 4) {
    format = GL_RGBA;
  } else {
    std::cerr << "Unsupported image channels: " << nrChannels << std::endl;
    stbi_image_free(image);
    return 0;
  }

  GLuint textureID;
  glGenTextures(1, &textureID);

  /* Bind it so any subsequent texture commands will configure the currently
   * bound texture. */
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);

  /* Set the texture wrapping/filtering options (on the currently bound texture
   * object. */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(image);

  return textureID;
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

  std::unique_ptr<ShaderProgram> shaderProgram = ShaderProgram::create(
      "assets/shaders/shader.vs", "assets/shaders/shader.fs");
  if (!shaderProgram) {
    glfwTerminate();
    return -1;
  }

  struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
  };

  std::vector<Vertex> vertices = {
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f } },

    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },

    { { -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f } },

    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f } },

    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },
    { {  0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f } },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f } },
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f } },

    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f } },
    { {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f } },
    { {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f } },
    { { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f } },
  };

  glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,   0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f,  -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f,  -3.5f),
    glm::vec3(-1.7f,  3.0f,  -7.5f),
    glm::vec3( 1.3f, -2.0f,  -2.5f),
    glm::vec3( 1.5f,  2.0f,  -2.5f),
    glm::vec3( 1.5f,  0.2f,  -1.5f),
    glm::vec3(-1.3f,  1.0f,  -1.5f),
  };

  Mesh mesh{vertices};

  GLuint textureID = loadTexture("assets/textures/container.jpg");
  if (textureID == 0) {
    glfwTerminate();
    return -1;
  }

  /* Don't forget to activate/use the shader before setting uniforms! */
  shaderProgram->use();
  shaderProgram->uniform("ourTexture", 0);

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Texture units allows for multiple textures on a single shader program by
     * binding multiple textures, each to a different texture unit. */
    glActiveTexture(GL_TEXTURE0);
    /* After activating a texture unit, a subsequent `glBindTexture` call will
     * bind that texture to the currently active texture unit. */
    glBindTexture(GL_TEXTURE_2D, textureID);

    shaderProgram->use();
    shaderProgram->uniform("viewMatrix", camera.getViewMatrix());
    shaderProgram->uniform(
      "projectionMatrix",
      camera.getProjectionMatrix(
        static_cast<float>(windowWidth) / windowHeight, 0.1f, 100.0f));

    for (int i = 0, n = sizeof cubePositions / sizeof cubePositions[0]; i < n; ++i) {
      float angle = 20.0f * i;
      glm::mat4 modelMatrix = glm::mat4(1.0f);
      modelMatrix = glm::translate(modelMatrix, cubePositions[i]);
      modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      shaderProgram->uniform("modelMatrix", modelMatrix);

      mesh.draw();
    }

    glfwSwapBuffers(window);
    /* The `glfwPollEvents` function checks if any events are triggered,
    * updates the window state, and calls the corresponding functions (which
    * we can register via callback methods). */
    glfwPollEvents();
  }

  glDeleteTextures(1, &textureID);

  glfwTerminate();

  return 0;
}
