#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class ShaderProgram {
  friend void swap(ShaderProgram& lhs, ShaderProgram& rhs) noexcept;

public:
  ShaderProgram(const ShaderProgram&) = delete;

  ShaderProgram(ShaderProgram&& other) noexcept
    : programID(other.programID) {
    other.programID = 0;
  }

  ShaderProgram& operator=(ShaderProgram other) noexcept {
    swap(*this, other);
    return *this;
  }

  ~ShaderProgram(void);

  static std::unique_ptr<ShaderProgram> create(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);

  GLuint getID(void) const {
    return programID;
  }

  void use(void) const;

  void uniform(const std::string& name, GLint value) const {
    glUniform1i(getUniformLocation(name), value);
  }

  void uniform(const std::string& name, GLfloat value) const {
    glUniform1f(getUniformLocation(name), value);
  }

  void uniform(const std::string& name, GLfloat x, GLfloat y, GLfloat z) const {
    glUniform3f(getUniformLocation(name), x, y, z);
  }

  void uniform(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
  }

  void uniform(const std::string& name, const glm::mat3& value) const {
    glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
  }

  void uniform(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
  }

private:
  explicit ShaderProgram(GLuint programID) : programID(programID) {}

  GLint getUniformLocation(const std::string& name) const;

  GLuint programID;
  mutable std::unordered_map<std::string, GLint> uniformLocationCache;
};

inline void swap(ShaderProgram& lhs, ShaderProgram& rhs) noexcept {
  using std::swap;
  swap(lhs.programID, rhs.programID);
}
