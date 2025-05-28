#include <iostream>
#include <memory>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

  GLfloat vertices[] = {
    // positions        // colors
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
     0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // top
  };

  /* A *vertex buffer object (VBO)* can store a large number of vertices in the
   * GPU memory. A *vertex array object (VAO)* stores our vertex attribute
   * configuration and which VBO to use. */
  GLuint VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  /* By binding a VAO, all subsequent VBO, EBO and vertex attribute calls will
   * be stored inside the VAO. */
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  /* `glBufferData` is a function specifically targeted to copy user-defined
   * data into the currently bound buffer. */
  glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

  /* We can tell OpenGL how it should interpret the vertex data (per vertex
   * attribute) using `glVertexAttribPointer`. */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
  /* Enable the vertex attribute with `glEnableVertexAttribArray`. */
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shaderProgram->use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    /* The `glfwPollEvents` function checks if any events are triggered,
     * updates the window state, and calls the corresponding functions (which
     * we can register via callback methods). */
    glfwPollEvents();
  }

  /* Optional: de-allocate all resources once they've outlived their purpose. */
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  glfwTerminate();

  return 0;
}
