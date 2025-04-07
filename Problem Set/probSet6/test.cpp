#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Window dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Test", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the OpenGL context current
    glfwMakeContextCurrent(window);

    // Get framebuffer size
    int screenW, screenH;
    glfwGetFramebufferSize(window, &screenW, &screenH);
    float aspectRatio = (float)screenW / (float)screenH;

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Setup projection matrix
    glMatrixMode(GL_PROJECTION);
    glm::mat4 P = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    glLoadMatrixf(glm::value_ptr(P));

    // Rendering loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw triangles
        glBegin(GL_TRIANGLES);
        glColor4f(1.0f, 0.0f, 0.0f, 0.5f); // Red with transparency
        glVertex3f(0.0f, 1.0f, -3.0f);
        glVertex3f(-1.0f, 1.0f, -3.0f);
        glVertex3f(-1.0f, -1.0f, -3.0f);
        glVertex3f(0.0f, -1.0f, -3.0f);
        glVertex3f(0.0f, 1.0f, -3.0f);
        glVertex3f(-1.0f, -1.0f, -3.0f);

        glColor4f(0.0f, 1.0f, 0.0f, 0.3f); // Green with transparency
        glVertex3f(-0.5f, 0.0f, -5.0f);
        glVertex3f(-0.5f, -1.0f, -5.0f);
        glVertex3f(1.f, -1.0f, -5.0f);
        glVertex3f(1.f, 0.0f, -5.0f);
        glVertex3f(-0.5f, 0.0f, -5.0f);
        glVertex3f(1.f, -1.0f, -5.0f);
        glEnd();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup and exit
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
