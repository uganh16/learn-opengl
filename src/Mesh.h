#pragma once

#include <algorithm>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoords;
};

class Mesh {
public:
  explicit Mesh(const std::vector<Vertex>& vertices);
  Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);

  Mesh(const Mesh&) = delete;

  Mesh(Mesh&& other) noexcept
    : VAO(other.VAO)
    , VBO(other.VBO)
    , EBO(other.EBO)
    , count(other.count) {
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
    other.count = 0;
  }

  ~Mesh(void);

  Mesh& operator=(Mesh other) noexcept {
    swap(*this, other);
    return *this;
  }

  friend void swap(Mesh& lhs, Mesh& rhs) noexcept {
    using std::swap;
    swap(lhs.VAO, rhs.VAO);
    swap(lhs.VBO, rhs.VBO);
    swap(lhs.EBO, rhs.EBO);
    swap(lhs.count, rhs.count);
  }

  void draw(void) const;

private:
  void setupVertices(const std::vector<Vertex>& vertices);
  void setupIndices(const std::vector<unsigned int>& indices);
  void clearState(void);

  GLuint VAO;
  GLuint VBO, EBO;
  GLsizei count;
};
