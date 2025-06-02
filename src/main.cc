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

  /* Create a framebuffer object and bind it. */
  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  /* Create a texture image that we attach as a color attachment to the
   * framebuffer. We set the texture's dimensions equal to the width and height
   * of the window and keep its data uninitialized. */
  GLuint texColorBufferID;
  glGenTextures(1, &texColorBufferID);
  glBindTexture(GL_TEXTURE_2D, texColorBufferID);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  /* Attach it to currently bound framebuffer object. */
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBufferID, 0);

  /* We also want to make sure OpenGL is able to do depth testing (and
   * optionally stencil testing) so we have to make sure to add a depth (and
   * stencil) attachment to the buffer. Since we'll only be sampling the color
   * buffer and not the other buffers we can create a renderbuffer object for
   * this purpose. */
  GLuint rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  /* Attach the renderbuffer object to the depth and stencil attachment of the
   * framebuffer. */
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << " Framebuffer is not complete!" << std::endl;
    glfwTerminate();
    return -1;
  }

  /* Make the default framebuffer active by binding to 0. */
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  std::unique_ptr<ShaderProgram> shaderProgram = ShaderProgram::create(
    "assets/shaders/defaultShader.vs", "assets/shaders/defaultShader.fs");
  std::unique_ptr<ShaderProgram> screenShaderProgram = ShaderProgram::create(
    "assets/shaders/screenShader.vs", "assets/shaders/screenShader.fs");
  if (!shaderProgram || !screenShaderProgram) {
    glfwTerminate();
    return -1;
  }

  GLuint cubeTextureID = TextureLoader::load("assets/textures/container.jpg");
  GLuint floorTextureID = TextureLoader::load("assets/textures/metal.png");
  if (cubeTextureID == 0 || floorTextureID == 0) {
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
  }, {
    { cubeTextureID, "texture0" }
  });

  Mesh floor(std::vector<Vertex>{
    { {  5.0f, -0.5f,  5.0f }, { 2.0f, 0.0f } },
    { { -5.0f, -0.5f,  5.0f }, { 0.0f, 0.0f } },
    { { -5.0f, -0.5f, -5.0f }, { 0.0f, 2.0f } },

    { {  5.0f, -0.5f,  5.0f }, { 2.0f, 0.0f } },
    { { -5.0f, -0.5f, -5.0f }, { 0.0f, 2.0f } },
    { {  5.0f, -0.5f, -5.0f }, { 2.0f, 2.0f } },
  }, {
    { floorTextureID, "texture0" }
  });

  struct ScreenVertex {
    glm::vec2 position;
    glm::vec2 texCoord;
  };

  Mesh quad(std::vector<ScreenVertex>{
    { { -1.0f,  1.0f }, { 0.0f,  1.0f } },
    { { -1.0f, -1.0f }, { 0.0f,  0.0f } },
    { {  1.0f, -1.0f }, { 1.0f,  0.0f } },

    { { -1.0f,  1.0f }, { 0.0f,  1.0f } },
    { {  1.0f, -1.0f }, { 1.0f,  0.0f } },
    { {  1.0f,  1.0f }, { 1.0f,  1.0f } },
  }, {
    { texColorBufferID, "screenTexture" }
  });

  std::vector<glm::mat3> postProcessingTransform = {
    glm::mat3(
       0.5f,  0.0f, 0.0f,
       0.0f,  0.5f, 0.0f,
      -0.5f,  0.5f, 1.0f
    ),
    glm::mat3(
      0.5f,  0.0f, 0.0f,
      0.0f,  0.5f, 0.0f,
      0.5f,  0.5f, 1.0f
    ),
    glm::mat3(
      0.5f,  0.0f, 0.0f,
      0.0f,  0.5f, 0.0f,
     -0.5f, -0.5f, 1.0f
    ),
    glm::mat3(
      0.5f,  0.0f, 0.0f,
      0.0f,  0.5f, 0.0f,
      0.5f, -0.5f, 1.0f
    ),
  };

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    /* Bind to framebuffer and draw scene as we normally would to color texture. */
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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

    /* Now bind back to the default framebuffer and draw a quad plane with the
     * attached framebuffer color texture. */
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_DEPTH_TEST);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    screenShaderProgram->use();

    for (int i = 0; i < 4; ++i) {
      screenShaderProgram->uniform("transform", postProcessingTransform[i]);
      screenShaderProgram->uniform("postProcessingType", (GLint)i);
      quad.draw(*screenShaderProgram);
    }

    glEnable(GL_DEPTH_TEST);

    glfwSwapBuffers(window);
    /* The `glfwPollEvents` function checks if any events are triggered,
    * updates the window state, and calls the corresponding functions (which
    * we can register via callback methods). */
    glfwPollEvents();
  }

  glDeleteRenderbuffers(1, &rbo);
  glDeleteTextures(1, &texColorBufferID);
  glDeleteFramebuffers(1, &fbo);

  glfwTerminate();

  return 0;
}
