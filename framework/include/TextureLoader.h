#pragma once

#include <string>
#include <unordered_map>

#include <glad/glad.h>

class TextureLoader {
public:
  static GLuint load(const std::string& path) {
    return getInstance().loadTexture(path);
  }

  static GLuint loadCubemap(const std::string& directory) {
    return getInstance().loadCubemapTexture(directory);
  }

private:
  static TextureLoader& getInstance(void) {
    static TextureLoader instance;
    return instance;
  }

  TextureLoader(void) = default;
  TextureLoader(const TextureLoader&) = delete;

  ~TextureLoader(void);

  GLuint loadTexture(const std::string& path);

  GLuint loadCubemapTexture(const std::string& directory);

  std::unordered_map<std::string, GLuint> cache;
};
