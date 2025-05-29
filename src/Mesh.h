#pragma once

#include <algorithm>
#include <vector>

#include <boost/pfr.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Mesh {
public:
  template <typename VertexType>
  explicit Mesh(const std::vector<VertexType>& vertices)
    : EBO(0)
    , count(vertices.size()) {
    setupVertices(vertices);
    unbindBuffers();
  }

  template <typename VertexType>
  Mesh(const std::vector<VertexType>& vertices, const std::vector<GLuint>& indices)
    : count(indices.size()) {
    setupVertices(vertices);
    setupIndices(indices);
    unbindBuffers();
  }

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
  template <typename VertexType>
  void setupVertices(const std::vector<VertexType>& vertices);

  void setupIndices(const std::vector<unsigned int>& indices);

  void bindBuffers(void);
  void unbindBuffers(void);

  GLuint VAO;
  GLuint VBO, EBO;
  GLsizei count;
};

template <typename VertexType>
void Mesh::setupVertices(const std::vector<VertexType>& vertices) {
  bindBuffers();

  constexpr size_t stride = sizeof(VertexType);
  size_t offset = 0;

  /* `glBufferData` is a function specifically targeted to copy user-defined
   * data into the currently bound buffer. */
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * stride, vertices.data(), GL_STATIC_DRAW);

  boost::pfr::for_each_field(VertexType{}, [&](auto&& field, auto index) {
    using T = std::decay_t<decltype(field)>;

    /* We can tell OpenGL how it should interpret the vertex data (per vertex
     * attribute) using `glVertexAttribPointer`. */
    if constexpr (std::is_same_v<T, glm::vec2>) {
      glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    } else if constexpr (std::is_same_v<T, glm::vec3>) {
      glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    } else if constexpr (std::is_same_v<T, glm::vec4>) {
      glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    } else {
      static_assert(false, "All vertex attributes must be glm::vec2/3/4");
    }

    /* Enable the vertex attribute with `glEnableVertexAttribArray` as vertex
     * attributes are disabled by default. */
    glEnableVertexAttribArray(index);

    offset += sizeof(T);
  });
}
