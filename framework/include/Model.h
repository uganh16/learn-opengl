#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Mesh.h"

class aiMesh;
class aiNode;
class aiScene;

class ShaderProgram;

class Model {
public:
  static std::unique_ptr<Model> load(const std::string& path);

  void draw(const ShaderProgram& shaderProgram) const {
    for (const auto& mesh : meshes) {
      mesh.draw(shaderProgram);
    }
  }

private:
  Model(std::string directory, const aiScene* scene);

  void processNode(const aiNode* node, const aiScene* scene);
  void processMesh(const aiMesh* mesh, const aiScene* scene);

  std::string directory;
  std::vector<Mesh> meshes;
};
