#include <cmath>
#include <iostream>
#include <memory>

#include <assimp/version.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

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

GLuint loadCubemap(const std::string& directory) {
  const static std::vector<std::pair<GLenum, std::string>> faces = {
    { GL_TEXTURE_CUBE_MAP_POSITIVE_X,  "right.jpg" },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_X,   "left.jpg" },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Y,    "top.jpg" },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "bottom.jpg" },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Z,  "front.jpg" },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,   "back.jpg" },
  };

  GLuint textureID;
  glGenTextures(1, &textureID);

  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  stbi_set_flip_vertically_on_load(false);

  int width, height, nrChannels;
  stbi_uc* image;
  for (const auto& pair : faces) {
    std::string path = directory + "/" + pair.second;
    image = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (image != NULL) {
      glTexImage2D(pair.first, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
      stbi_image_free(image);
    } else {
      std::cerr << "Cubemap texture failed to load at path: " << path << std::endl;
    }
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

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

  printSystemInfo();

  glEnable(GL_DEPTH_TEST);

  std::unique_ptr<ShaderProgram> shaderProgram = ShaderProgram::create(
    "assets/shaders/defaultShader.vs", "assets/shaders/defaultShader.fs");
  std::unique_ptr<ShaderProgram> skyboxShaderProgram = ShaderProgram::create(
    "assets/shaders/skyboxShader.vs", "assets/shaders/skyboxShader.fs");
  if (!shaderProgram || !skyboxShaderProgram) {
    glfwTerminate();
    return -1;
  }

  GLuint cubeTextureID = TextureLoader::load("assets/textures/container.jpg");
  if (cubeTextureID == 0) {
    glfwTerminate();
    return -1;
  }

  GLuint cubemapTextureID = loadCubemap("assets/textures/skybox");

  struct SkyboxVertex {
    glm::vec3 position;
  };

  Mesh skybox(std::vector<SkyboxVertex>{
    { { -1.0f,  1.0f, -1.0f } },
    { { -1.0f, -1.0f, -1.0f } },
    { {  1.0f, -1.0f, -1.0f } },
    { {  1.0f, -1.0f, -1.0f } },
    { {  1.0f,  1.0f, -1.0f } },
    { { -1.0f,  1.0f, -1.0f } },

    { { -1.0f, -1.0f,  1.0f } },
    { { -1.0f, -1.0f, -1.0f } },
    { { -1.0f,  1.0f, -1.0f } },
    { { -1.0f,  1.0f, -1.0f } },
    { { -1.0f,  1.0f,  1.0f } },
    { { -1.0f, -1.0f,  1.0f } },

    { {  1.0f, -1.0f, -1.0f } },
    { {  1.0f, -1.0f,  1.0f } },
    { {  1.0f,  1.0f,  1.0f } },
    { {  1.0f,  1.0f,  1.0f } },
    { {  1.0f,  1.0f, -1.0f } },
    { {  1.0f, -1.0f, -1.0f } },

    { { -1.0f, -1.0f,  1.0f } },
    { { -1.0f,  1.0f,  1.0f } },
    { {  1.0f,  1.0f,  1.0f } },
    { {  1.0f,  1.0f,  1.0f } },
    { {  1.0f, -1.0f,  1.0f } },
    { { -1.0f, -1.0f,  1.0f } },

    { { -1.0f,  1.0f, -1.0f } },
    { {  1.0f,  1.0f, -1.0f } },
    { {  1.0f,  1.0f,  1.0f } },
    { {  1.0f,  1.0f,  1.0f } },
    { { -1.0f,  1.0f,  1.0f } },
    { { -1.0f,  1.0f, -1.0f } },

    { { -1.0f, -1.0f, -1.0f } },
    { { -1.0f, -1.0f,  1.0f } },
    { {  1.0f, -1.0f, -1.0f } },
    { {  1.0f, -1.0f, -1.0f } },
    { { -1.0f, -1.0f,  1.0f } },
    { {  1.0f, -1.0f,  1.0f } },
  }, {
    { cubemapTextureID, "skybox" }
  });

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

    shaderProgram->use();
    shaderProgram->uniform("projectionMatrix", projectionMatrix);
    shaderProgram->uniform("viewMatrix", viewMatrix);
    shaderProgram->uniform("modelMatrix", glm::mat4(1.0f));
    cube.draw(*shaderProgram);

    /* So to give us a slight performance boost we're going to render the skybox
     * last. This way, the depth buffer is completely filled with all the
     * scene's depth values so we only have to render the skybox's fragments
     * wherever the early depth test passes, greatly reducing the number of
     * fragment shader calls. */
    skyboxShaderProgram->use();
    skyboxShaderProgram->uniform("projectionMatrix", projectionMatrix);
    skyboxShaderProgram->uniform("viewMatrix", viewMatrix);
    /* We do have to change the depth function a little by setting it to
     * `GL_LEQUAL` instead of the default `GL_LESS`. The depth buffer will be
     * filled with values of 1.0 for the skybox, so we need to make sure the
     * skybox passes the depth tests with values *less than or equal* to the
     * depth buffer instead of *less than*. */
    glDepthFunc(GL_LEQUAL);
    skybox.draw(*skyboxShaderProgram);
    glDepthFunc(GL_LESS);

    glfwSwapBuffers(window);
    /* The `glfwPollEvents` function checks if any events are triggered,
    * updates the window state, and calls the corresponding functions (which
    * we can register via callback methods). */
    glfwPollEvents();
  }

  glDeleteTextures(1, &cubemapTextureID);

  glfwTerminate();

  return 0;
}
