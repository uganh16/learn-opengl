#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices)
  : EBO(0)
  , count(vertices.size()) {
  setupVertices(vertices);
  clearState();
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
  : count(indices.size()) {
  setupVertices(vertices);
  setupIndices(indices);
  clearState();
}

Mesh::~Mesh(void) {
  if (VAO != 0) {
    glDeleteVertexArrays(1, &VAO);
  }
  if (VBO != 0) {
    glDeleteBuffers(1, &VBO);
  }
  if (EBO != 0) {
    glDeleteBuffers(1, &EBO);
  }
}

void Mesh::draw(void) const {
  glBindVertexArray(VAO);
  if (EBO != 0) {
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)0);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, count);
  }
  glBindVertexArray(0);
}

void Mesh::setupVertices(const std::vector<Vertex>& vertices) {
  /* A vertex buffer object (VBO) can store a large number of vertices in the
   * GPU memory.  */
  glGenVertexArrays(1, &VAO);

  /* By binding a VAO, all subsequent VBO, EBO and vertex attribute calls will
   * be stored inside the VAO. */
  glBindVertexArray(VAO);

  /* A vertex array object (VAO) stores our vertex attribute configuration and
   * which VBO to use. */
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  /* `glBufferData` is a function specifically targeted to copy user-defined
   * data into the currently bound buffer. */
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

  /* We can tell OpenGL how it should interpret the vertex data (per vertex
   * attribute) using `glVertexAttribPointer`. */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
  /* Enable the vertex attribute with `glEnableVertexAttribArray` as vertex
   * attributes are disabled by default. */
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
  glEnableVertexAttribArray(2);
}

void Mesh::setupIndices(const std::vector<unsigned int>& indices) {
  /* An *element buffer object (EBO)* is a buffer that stores indices that
   * OpenGL uses to decide what vertices to draw. */
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

void Mesh::clearState(void) {
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  if (EBO != 0) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
}
