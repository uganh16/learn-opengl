#include "Model.h"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "TextureLoader.h"

std::unique_ptr<Model> Model::load(const std::string& path) {
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
    return nullptr;
  }

  auto sep = path.find_last_of('/');
  if (sep == std::string::npos) {
    sep = 0;
  }

  return std::unique_ptr<Model>(new Model(path.substr(0, sep), scene));
}

Model::Model(std::string directory, const aiScene* scene)
  : directory(std::move(directory)) {
  processNode(scene->mRootNode, scene);
}

void Model::processNode(const aiNode* node, const aiScene* scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
    const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    processMesh(mesh, scene);
  }

  for (unsigned int i = 0; i < node->mNumChildren; ++i) {
    processNode(node->mChildren[i], scene);
  }
}

void Model::processMesh(const aiMesh* mesh, const aiScene* scene) {
  struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
  };

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

  /* Walk through each of the mesh's vertices. */
  for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
    Vertex vertex;
    /* Positions */
    vertex.position = glm::vec3(
      mesh->mVertices[i].x,
      mesh->mVertices[i].y,
      mesh->mVertices[i].z
    );
    /* Normals */
    if (mesh->HasNormals()) {
      vertex.normal = glm::vec3(
        mesh->mNormals[i].x,
        mesh->mNormals[i].y,
        mesh->mNormals[i].z
      );
    }
    /* Texture coordinates */
    if (mesh->mTextureCoords[0]) {
      vertex.texCoord = glm::vec2(
        mesh->mTextureCoords[0][i].x,
        mesh->mTextureCoords[0][i].y
      );
    } else {
      vertex.texCoord = glm::vec2(0.0f);
    }
    vertices.push_back(vertex);
  }

  /* Now walk through each of the mesh's faces and retrieve the corresponding
   * vertex indices. */
  for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
    aiFace face = mesh->mFaces[i];
    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  static const std::unordered_map<aiTextureType, std::string> supportedTextureTypes = {
    { aiTextureType_AMBIENT, "ambient" },
    { aiTextureType_DIFFUSE, "diffuse" },
    { aiTextureType_SPECULAR, "specular" },
    { aiTextureType_HEIGHT, "height" },
  };

  /* Process materials. */
  const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
  std::unordered_map<std::string, int> textureNrs;
  for (const auto& pair : supportedTextureTypes) {
    for (unsigned int i = 0, n = material->GetTextureCount(pair.first); i < n; ++i) {
      aiString str;
      material->GetTexture(pair.first, i, &str);
      std::string path = directory + "/" + str.C_Str();
      GLuint textureID = TextureLoader::load(path);
      if (textureID != 0) {
        /* Retrieve texture number. */
        int number = textureNrs[pair.second]++;
        textures.push_back({ textureID, "material." + pair.second + std::to_string(number), path });
      }
    }
  }

  meshes.emplace_back(vertices, indices, textures);
}
