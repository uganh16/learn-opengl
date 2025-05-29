#include "Mesh.h"

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

void Mesh::setupIndices(const std::vector<unsigned int>& indices) {
  /* An *element buffer object (EBO)* is a buffer that stores indices that
   * OpenGL uses to decide what vertices to draw. */
  glGenBuffers(1, &EBO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}

void Mesh::bindBuffers(void) {
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
}

void Mesh::unbindBuffers(void) {
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  if (EBO != 0) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
}
