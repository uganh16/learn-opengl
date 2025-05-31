#include "ShaderProgram.h"

#include <fstream>
#include <iostream>
#include <sstream>

static bool loadShaderFile(const std::string& path, std::string& outSource) {
  std::ifstream file(path);
  if (!file.is_open()) {
    return false;
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  outSource = buffer.str();
  return true;
}

static GLuint compileShader(GLenum type, const std::string& shaderSource) {
  GLuint shaderID = glCreateShader(type);
  const GLchar* source = shaderSource.c_str();
  glShaderSource(shaderID, 1, &source, NULL);
  glCompileShader(shaderID);
  GLint success;
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar infoLog[512];
    glGetShaderInfoLog(shaderID, sizeof infoLog, NULL, infoLog);
    std::cerr << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
              << " shader compilation failed:\n"
              << infoLog << std::endl;
    glDeleteShader(shaderID);
    return 0;
  }
  return shaderID;
}

static GLuint linkProgram(GLuint vertexShaderID, GLuint fragmentShaderID) {
  GLuint programID = glCreateProgram();
  glAttachShader(programID, vertexShaderID);
  glAttachShader(programID, fragmentShaderID);
  glLinkProgram(programID);
  GLint success;
  glGetProgramiv(programID, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar infoLog[512];
    glGetProgramInfoLog(programID, sizeof infoLog, NULL, infoLog);
    std::cerr << "Shader program linking failed:\n" << infoLog << std::endl;
    glDeleteProgram(programID);
    return 0;
  }
  return programID;
}

ShaderProgram::~ShaderProgram(void) {
  if (programID != 0) {
    glDeleteProgram(programID);
  }
}

std::unique_ptr<ShaderProgram> ShaderProgram::create(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
  std::string shaderSource;

  if (!loadShaderFile(vertexShaderPath, shaderSource)) {
    std::cerr << "Failed to load vertex shader: " << vertexShaderPath << std::endl;
    return nullptr;
  }

  GLuint vertexShaderID = compileShader(GL_VERTEX_SHADER, shaderSource);
  if (vertexShaderID == 0) {
    return nullptr;
  }

  if (!loadShaderFile(fragmentShaderPath, shaderSource)) {
    std::cerr << "Failed to load fragment shader: " << fragmentShaderPath << std::endl;
    glDeleteShader(vertexShaderID);
    return nullptr;
  }

  GLuint fragmentShaderID = compileShader(GL_FRAGMENT_SHADER, shaderSource);
  if (fragmentShaderID == 0) {
    glDeleteShader(vertexShaderID);
    return nullptr;
  }

  GLuint programID = linkProgram(vertexShaderID, fragmentShaderID);

  /* Don't forget to delete the shader objects once we've linked them into the
   * program object; we no longer need them anymore. */
  glDeleteShader(fragmentShaderID);
  glDeleteShader(vertexShaderID);

  if (programID == 0) {
    return nullptr;
  }

  return std::unique_ptr<ShaderProgram>(new ShaderProgram(programID));
}

void ShaderProgram::use(void) const {
  glUseProgram(programID);
}

GLint ShaderProgram::getUniformLocation(const std::string& name) const {
  auto it = uniformLocationCache.find(name);
  if (it != uniformLocationCache.end()) {
    return it->second;
  }
  GLint location = glGetUniformLocation(programID, name.c_str());
  if (location == -1) {
    std::cerr << "Uniform '" << name << "' not found in shader program (ID: " << programID << ")" << std::endl;
  }
  uniformLocationCache[name] = location;
  return location;
}
