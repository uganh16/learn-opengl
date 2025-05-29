#include <iostream>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Mesh.h"
#include "ShaderProgram.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
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

  GLFWwindow* window = glfwCreateWindow(800, 600, "Learn OpenGL", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  /* Tell GLFW to make the context of our window the main context on the
   * current thread. */
  glfwMakeContextCurrent(window);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    return -1;
  }

  printSystemInfo();

  std::unique_ptr<ShaderProgram> shaderProgram = ShaderProgram::create(
      "assets/shaders/shader.vs", "assets/shaders/shader.fs");
  if (!shaderProgram) {
    glfwTerminate();
    return -1;
  }

  struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texCoord;
  };

  std::vector<Vertex> vertices = {
    { {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } }, // top right
    { {  0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } }, // bottom right
    { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } }, // bottom left
    { { -0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } }, // top left
  };

  Mesh mesh{
    vertices,
    {
      0, 1, 3,
      1, 2, 3,
    }
  };

  GLuint textureID = loadTexture("assets/textures/container.jpg");
  if (textureID == 0) {
    glfwTerminate();
    return -1;
  }

  /* Don't forget to activate/use the shader before setting uniforms! */
  shaderProgram->use();
  shaderProgram->uniform("ourTexture", 0);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Texture units allows for multiple textures on a single shader program by
     * binding multiple textures, each to a different texture unit. */
    glActiveTexture(GL_TEXTURE0);
    /* After activating a texture unit, a subsequent `glBindTexture` call will
     * bind that texture to the currently active texture unit. */
    glBindTexture(GL_TEXTURE_2D, textureID);

    shaderProgram->use();
    mesh.draw();

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
