#include "TextureLoader.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

TextureLoader::~TextureLoader(void) {
  for (auto& pair : cache) {
    glDeleteTextures(1, &pair.second);
  }
  cache.clear();
}

GLuint TextureLoader::loadTexture(const std::string& path) {
  auto it = cache.find(path);
  if (it != cache.end()) {
    return it->second;
  }

  /* OpenGL's coordinate system has the Y-axis pointing upward (0 at the
   * bottom), while most image formats store pixel data with the Y-axis pointing
   * downward (0 at the top). Calling stbi_set_flip_vertically_on_load(true)
   * before loading an image with stb_image.h flips the image data vertically,
   * aligning it with OpenGL's coordinate system for correct rendering. */
  stbi_set_flip_vertically_on_load(true);

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

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);

  /* Set the texture wrapping/filtering options (on the currently bound texture
   * object. */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(image);
  glBindTexture(GL_TEXTURE_2D, 0);

  cache[path] = textureID;

  return textureID;
}
