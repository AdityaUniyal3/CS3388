#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

// camera.cpp

#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

// Global static variables for spherical camera
static float gRadius   = 20.0f; // ~ distance for an initial position near (5,5,5)
static float gTheta    = 0.785f; // about 45 degrees in radians
static float gPhi      = 0.615f; // about 35 deg above xz-plane (pick what feels good)

// For detecting mouse drag
static bool  gMouseHeld = false;
static float gLastMouseX = 0.0f;
static float gLastMouseY = 0.0f;

// Sensitivity factors (tweak these to taste)
static float gThetaSensitivity = 0.005f;
static float gPhiSensitivity   = 0.005f;
static float gZoomSpeed        = 0.05f;

// clamp on phi so you can’t “flip” the camera upside down:
static float gPhiMin = -1.57f;  // -90 deg
static float gPhiMax =  1.57f;  // +90 deg


void cameraFirstPerson(GLFWwindow* window, glm::mat4& viewMatrix, float speed)
{
    // Example: up arrow => move closer, down arrow => move farther
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        gRadius -= (speed * gZoomSpeed);
        if (gRadius < 0.5f) gRadius = 0.5f; // avoid zero or negative
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        gRadius += (speed * gZoomSpeed);
    }
    
    // Now compute camera position in Cartesian coords from (r, theta, phi).
    // We'll assume y is "up," so let's treat phi as the angle above the x-z plane.
    
    float x = gRadius * cosf(gPhi) * cosf(gTheta);
    float y = gRadius * sinf(gPhi);
    float z = gRadius * cosf(gPhi) * sinf(gTheta);
    
    glm::vec3 eye(x, y, z);
    glm::vec3 center(0.0f, 0.0f, 0.0f); // looking toward origin
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    
    viewMatrix = glm::lookAt(eye, center, up);
}

// Called automatically by GLFW when a mouse button is pressed/released.
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            gMouseHeld = true;
            // Record where the mouse was when they pressed
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            gLastMouseX = (float)x;
            gLastMouseY = (float)y;
        }
        else if (action == GLFW_RELEASE)
        {
            gMouseHeld = false;
        }
    }
}

// Called automatically by GLFW whenever the mouse moves.
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gMouseHeld)
    {
        float dx = (float)xpos - gLastMouseX;
        float dy = (float)ypos - gLastMouseY;
        gLastMouseX = (float)xpos;
        gLastMouseY = (float)ypos;
        
        // Horizontal drag -> change theta
        gTheta -= dx * gThetaSensitivity;  // minus sign if you want "drag left => rotate left"
        // Vertical drag -> change phi
        gPhi   -= dy * gPhiSensitivity;
        
        // Optionally clamp phi so we don't flip the camera upside-down
        if (gPhi < gPhiMin) gPhi = gPhiMin;
        if (gPhi > gPhiMax) gPhi = gPhiMax;
    }
}
