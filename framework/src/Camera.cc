#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch)
  : position(position)
  , worldUp(worldUp)
  , yaw(yaw)
  , pitch(pitch)
  , movementSpeed(2.5f)
  , mouseSensitivity(0.1f)
  , fovy(45.0f) {
  updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix(void) const {
  return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix(float aspect, float near, float far) const {
  return glm::perspective(glm::radians(fovy), aspect, near, far);
}

void Camera::processMovement(CameraMovement direction, float deltaTime) {
  float velocity = movementSpeed * deltaTime;
  switch (direction) {
  case CameraMovement::FORWARD:
    position += front * velocity;
    break;
  case CameraMovement::BACKWARD:
    position -= front * velocity;
    break;
  case CameraMovement::LEFT:
    position -= right * velocity;
    break;
  case CameraMovement::RIGHT:
    position += right * velocity;
    break;
  }
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
  yaw   += xoffset * mouseSensitivity;
  pitch += yoffset * mouseSensitivity;

  if (constrainPitch) {
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
  }

  updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
  fovy = glm::clamp(fovy - yoffset, 1.0f, 90.0f);
}

void Camera::updateCameraVectors(void) {
  /* Calculate the new front vector. */
  front.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
  front.y = std::sin(glm::radians(pitch));
  front.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
  front = glm::normalize(front);
  /* Also re-calculate the right and up vector. */
  right = glm::normalize(glm::cross(front, worldUp));
  up    = glm::normalize(glm::cross(right, front));
}
