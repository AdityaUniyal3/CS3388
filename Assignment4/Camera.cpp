#define GLM_ENABLE_EXPERIMENTAL
#include "Camera.h"
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <glm/gtx/string_cast.hpp>
#include <sstream>
#include <string>


Camera::Camera()
    : position(0.5f, 0.4f, 0.5f),
      yaw(90.0f),    // Changed from 0.0f to 90.0f
      pitch(0.0f),
      movementSpeed(5.0f),
      rotationSpeed(60.0f)
{
}

void Camera::update(bool up, bool down, bool left, bool right, float deltaTime) {
    // Compute the forward direction from yaw (ignoring pitch for simplicity)
    glm::vec3 forward;
    forward.x = cos(glm::radians(yaw));
    forward.y = 0.0f;
    forward.z = -sin(glm::radians(yaw));
    forward = glm::normalize(forward);
    
    // Move forward/backward
    if (up)
        position += forward * movementSpeed * deltaTime;
    if (down)
        position -= forward * movementSpeed * deltaTime;
    
    // Rotate left/right
    if (left)
        yaw += rotationSpeed * deltaTime;
    if (right)
        yaw -= rotationSpeed * deltaTime;
}

void Camera::setPosition(const glm::vec3 &pos) {
    position = pos;
}

glm::vec3 Camera::getPosition() {
    return position;
}

void Camera::setYaw(float y) {
    yaw = y;
}


glm::mat4 Camera::getViewMatrix() const {
    glm::vec3 forward;
    forward.x = cos(glm::radians(yaw));
    forward.y = 0.0f;
    forward.z = -sin(glm::radians(yaw));
    forward = glm::normalize(forward);
    
    return glm::lookAt(position, position + forward, glm::vec3(0.0f, 1.0f, 0.0f));
}

std::string Camera::getDebugInfo() const {
    std::ostringstream oss;
    oss << "Camera Position: " << glm::to_string(position) << "\n"
        << "Yaw: " << yaw << "\n"
        << "Pitch: " << pitch << "\n"
        << "View Matrix:\n" << glm::to_string(getViewMatrix()) << "\n";
    return oss.str();
}
