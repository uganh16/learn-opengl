#pragma once

#include <glm/glm.hpp>

enum class CameraMovement {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
};

class Camera {
public:
  explicit Camera(glm::vec3 position, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);

  glm::mat4 getViewMatrix(void) const;
  glm::mat4 getProjectionMatrix(float aspect, float near = 0.1f, float far = 100.0f) const;

  void processMovement(CameraMovement direction, float deltaTime);

  void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

  void processMouseScroll(float yoffset);

  void setMovementSpeed(float speed) {
    movementSpeed = glm::max(speed, 0.0f);
  }

  void setSensitivity(float sensitivity) {
    mouseSensitivity = glm::max(sensitivity, 0.0f);
  }

  const glm::vec3& getPosition(void) const {
    return position;
  }

  const glm::vec3& getFront(void) const {
    return front;
  }

private:
  void updateCameraVectors(void);

  glm::vec3 position;
  glm::vec3 worldUp;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;

  float yaw;
  float pitch;

  float movementSpeed;
  float mouseSensitivity;

  float fovy;
};
