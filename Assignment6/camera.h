#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Update the view matrix using first-person style camera controls.
// window: pointer to the GLFW window (to capture input)
// viewMatrix: reference to the view matrix to update
// speed: a parameter to control camera sensitivity
void cameraFirstPerson(GLFWwindow* window, glm::mat4& viewMatrix, float speed);

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

#endif // CAMERA_H
