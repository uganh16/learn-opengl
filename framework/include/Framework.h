#pragma once

#include <memory>

#include <glm/glm.hpp>

class App {
public:
  virtual ~App(void) = default;

  virtual void update(float deltaTime) = 0;

protected:
  glm::vec3 getCameraPosition(void) const;
  glm::vec3 getCameraFront(void) const;

  glm::mat4 getViewMatrix(void) const;
  glm::mat4 getProjectionMatrix(void) const;
};

extern std::unique_ptr<App> createApp(void);

#define REGISTER_APP(AppClass)                                                  \
  std::unique_ptr<App> createApp(void) {                                        \
    return std::make_unique<AppClass>();                                        \
  }
