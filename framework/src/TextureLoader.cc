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

GLuint TextureLoader::loadCubemapTexture(const std::string& directory) {
  const static std::vector<std::pair<GLenum, std::string>> faces = {
    { GL_TEXTURE_CUBE_MAP_POSITIVE_X,  "right.jpg" },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_X,   "left.jpg" },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Y,    "top.jpg" },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "bottom.jpg" },
    { GL_TEXTURE_CUBE_MAP_POSITIVE_Z,  "front.jpg" },
    { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,   "back.jpg" },
  };

  GLuint textureID;
  glGenTextures(1, &textureID);

  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  stbi_set_flip_vertically_on_load(false);

  int width, height, nrChannels;
  stbi_uc* image;
  for (const auto& pair : faces) {
    std::string path = directory + "/" + pair.second;
    image = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (image != NULL) {
      glTexImage2D(pair.first, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
      stbi_image_free(image);
    } else {
      std::cerr << "Cubemap texture failed to load at path: " << path << std::endl;
    }
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  cache[directory] = textureID;

  return textureID;
}
