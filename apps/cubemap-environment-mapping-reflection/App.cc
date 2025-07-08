#include <glad/glad.h>

#include "Framework.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "TextureLoader.h"

class EnvironmentMappingApp : public App {
public:
  EnvironmentMappingApp(void);

  virtual void update(float deltaTime) override;

private:
  std::unique_ptr<ShaderProgram> shaderProgram;
  std::unique_ptr<ShaderProgram> skyboxShaderProgram;

  Mesh skybox;
  Mesh cube;
};

REGISTER_APP(EnvironmentMappingApp)

static Mesh loadSkyboxMesh(void) {
  struct Vertex {
    glm::vec3 position;
  };

  return Mesh(std::vector<Vertex>{
    { { -1.0f,  1.0f, -1.0f } },
    { { -1.0f, -1.0f, -1.0f } },
    { {  1.0f, -1.0f, -1.0f } },
    { {  1.0f, -1.0f, -1.0f } },
    { {  1.0f,  1.0f, -1.0f } },
    { { -1.0f,  1.0f, -1.0f } },

    { { -1.0f, -1.0f,  1.0f } },
    { { -1.0f, -1.0f, -1.0f } },
    { { -1.0f,  1.0f, -1.0f } },
    { { -1.0f,  1.0f, -1.0f } },
    { { -1.0f,  1.0f,  1.0f } },
    { { -1.0f, -1.0f,  1.0f } },

    { {  1.0f, -1.0f, -1.0f } },
    { {  1.0f, -1.0f,  1.0f } },
    { {  1.0f,  1.0f,  1.0f } },
    { {  1.0f,  1.0f,  1.0f } },
    { {  1.0f,  1.0f, -1.0f } },
    { {  1.0f, -1.0f, -1.0f } },

    { { -1.0f, -1.0f,  1.0f } },
    { { -1.0f,  1.0f,  1.0f } },
    { {  1.0f,  1.0f,  1.0f } },
    { {  1.0f,  1.0f,  1.0f } },
    { {  1.0f, -1.0f,  1.0f } },
    { { -1.0f, -1.0f,  1.0f } },

    { { -1.0f,  1.0f, -1.0f } },
    { {  1.0f,  1.0f, -1.0f } },
    { {  1.0f,  1.0f,  1.0f } },
    { {  1.0f,  1.0f,  1.0f } },
    { { -1.0f,  1.0f,  1.0f } },
    { { -1.0f,  1.0f, -1.0f } },

    { { -1.0f, -1.0f, -1.0f } },
    { { -1.0f, -1.0f,  1.0f } },
    { {  1.0f, -1.0f, -1.0f } },
    { {  1.0f, -1.0f, -1.0f } },
    { { -1.0f, -1.0f,  1.0f } },
    { {  1.0f, -1.0f,  1.0f } },
  }, {
    { TextureLoader::loadCubemap("assets/textures/skybox"), true, "skybox" }
  });
}

static Mesh loadCubeMesh(void) {
  struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
  };

  return Mesh(std::vector<Vertex>{
    { { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },
    { {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },
    { {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },
    { {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },
    { { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },
    { { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f } },

    { { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },
    { {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },
    { {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },
    { {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },
    { { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },
    { { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f } },

    { { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f } },
    { { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f } },
    { { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f } },
    { { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f } },
    { { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f } },
    { { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f } },

    { {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f } },
    { {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f } },
    { {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f } },
    { {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f } },
    { {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f } },
    { {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f } },

    { { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f } },
    { {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f } },
    { {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f } },
    { {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f } },
    { { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f } },
    { { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f } },

    { { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f } },
    { {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f } },
    { {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f } },
    { {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f } },
    { { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f } },
    { { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f } },
  }, {
    { TextureLoader::loadCubemap("assets/textures/skybox"), true, "skybox" }
  });
}

EnvironmentMappingApp::EnvironmentMappingApp(void)
  : shaderProgram(ShaderProgram::create(
    "assets/shaders/environmentMappingShader.vs",
    "assets/shaders/environmentMappingShader.fs"))
  , skyboxShaderProgram(ShaderProgram::create(
    "assets/shaders/skyboxShader.vs",
    "assets/shaders/skyboxShader.fs"))
  , skybox(loadSkyboxMesh())
  , cube(loadCubeMesh()) {

}

void EnvironmentMappingApp::update(float deltaTime) {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 projectionMatrix = getProjectionMatrix();
  glm::mat4 viewMatrix = getViewMatrix();

  shaderProgram->use();
  shaderProgram->uniform("cameraPos", getCameraPosition());
  shaderProgram->uniform("projectionMatrix", projectionMatrix);
  shaderProgram->uniform("viewMatrix", viewMatrix);
  shaderProgram->uniform("modelMatrix", glm::mat4(1.0f));
  cube.draw(*shaderProgram);

  /* So to give us a slight performance boost we're going to render the skybox
   * last. This way, the depth buffer is completely filled with all the
   * scene's depth values so we only have to render the skybox's fragments
   * wherever the early depth test passes, greatly reducing the number of
   * fragment shader calls. */
  skyboxShaderProgram->use();
  skyboxShaderProgram->uniform("projectionMatrix", projectionMatrix);
  skyboxShaderProgram->uniform("viewMatrix", viewMatrix);
  /* We do have to change the depth function a little by setting it to
   * `GL_LEQUAL` instead of the default `GL_LESS`. The depth buffer will be
   * filled with values of 1.0 for the skybox, so we need to make sure the
   * skybox passes the depth tests with values *less than or equal* to the
   * depth buffer instead of *less than*. */
  glDepthFunc(GL_LEQUAL);
  skybox.draw(*skyboxShaderProgram);
  glDepthFunc(GL_LESS);
}
