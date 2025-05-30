#pragma once

#include <string>
#include <unordered_map>

#include <glad/glad.h>

class TextureLoader {
public:
  static GLuint load(const std::string& path) {
    static TextureLoader instance;
    return instance.loadTexture(path);
  }

private:
  TextureLoader(void) = default;
  TextureLoader(const TextureLoader&) = delete;

  ~TextureLoader(void);

  GLuint loadTexture(const std::string& path);

  std::unordered_map<std::string, GLuint> cache;
};
