#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const GLchar* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
void main() {
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)";

const GLchar* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
void main() {
  FragColor = vec4(1.0, 0.5, 0.2, 1.0);
}
)";

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

  GLint success;
  GLchar infoLog[512];

  GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShaderID);
  glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShaderID, sizeof infoLog, NULL, infoLog);
    std::cerr << "Vertex shader compilation failed:\n" << infoLog << std::endl;
    glDeleteShader(vertexShaderID);
    glfwTerminate();
    return -1;
  }

  GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShaderID);
  glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShaderID, sizeof infoLog, NULL, infoLog);
    std::cerr << "Fragment shader compilation failed:\n" << infoLog << std::endl;
    glDeleteShader(fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glfwTerminate();
    return -1;
  }

  GLuint shaderProgramID = glCreateProgram();
  glAttachShader(shaderProgramID, vertexShaderID);
  glAttachShader(shaderProgramID, fragmentShaderID);
  glLinkProgram(shaderProgramID);
  glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(fragmentShaderID, sizeof infoLog, NULL, infoLog);
    std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    glDeleteProgram(shaderProgramID);
    glDeleteShader(fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glfwTerminate();
    return -1;
  }

  glDeleteShader(fragmentShaderID);
  glDeleteShader(vertexShaderID);

  GLfloat vertices[] = {
     0.5f,  0.5f, 0.0f, // top right
     0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f,  0.5f, 0.0f, // top left
  };

  GLuint indices[] = {
    0, 1, 3,
    1, 2, 3,
  };

  /* A *vertex buffer object (VBO)* can store a large number of vertices in the
   * GPU memory. A *vertex array object (VAO)* stores our vertex attribute
   * configuration and which VBO to use. */
  GLuint VBO, VAO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  /* An *element buffer object (EBO)* is a buffer that stores indices that
   * OpenGL uses to decide what vertices to draw. */
  GLuint EBO;
  glGenBuffers(1, &EBO);

  /* By binding a VAO, all subsequent VBO, EBO and vertex attribute calls will
   * be stored inside the VAO. */
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  /* `glBufferData` is a function specifically targeted to copy user-defined
   * data into the currently bound buffer. */
  glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

  /* We can tell OpenGL how it should interpret the vertex data (per vertex
   * attribute) using `glVertexAttribPointer`. */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
  /* Enable the vertex attribute with `glEnableVertexAttribArray`. */
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  /* Note that: do *not* unbind the EBO while the VAO is active, otherwise it
   * doesn't have an EBO configured. */
  glBindVertexArray(0);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgramID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof indices / sizeof indices[0], GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    /* The `glfwPollEvents` function checks if any events are triggered,
     * updates the window state, and calls the corresponding functions (which
     * we can register via callback methods). */
    glfwPollEvents();
  }

  /* Optional: de-allocate all resources once they've outlived their purpose. */
  glDeleteBuffers(1, &EBO);
  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &VAO);
  glDeleteProgram(shaderProgramID);

  glfwTerminate();

  return 0;
}
