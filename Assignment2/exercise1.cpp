#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

#define PI 3.14159265358979323846

vector<pair<float, float>> vertices; // Store (x, y) from dog.txt
float circle_angles[8] = {0, 45, 90, 135, 180, 225, 270, 315}; // Fixed circle positions
float spin_angles[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // Local spin angles
float rotation_speed = 1.0f; // 1° per frame

// Function to load (x, y) vertices from dog.txt
void loadVertices(const char* filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }
    float x, y;
    while (file >> x >> y) {
        vertices.push_back({x, y});
    }
    file.close();
}

void drawDog(float cx, float cy, float rotationAngle) {
    glPushMatrix(); // Save the current transformation matrix

    // Step 1: Translate the dog to its fixed position on the circle
    glTranslatef(cx, cy, 0);

    // Step 2: Rotate the dog locally around its center
    glRotatef(rotationAngle, 0, 0, 1);

    // Step 3: Draw the dog using its vertices
    glBegin(GL_LINE_LOOP);
    for (auto vertex : vertices) {
        glVertex2f(vertex.first, vertex.second);
    }
    glEnd();

    glPopMatrix(); // Restore the transformation matrix
}


// OpenGL render function
void renderDogs() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the screen with white

    float cx = 30.0f;  // Circle center x
    float cy = 30.0f;  // Circle center y
    float radius = 25.0f; // Circle radius

    glColor3f(0, 0, 0); // Set color to black for the dogs

    // Draw 8 dogs around the circle
    for (int i = 0; i < 8; i++) {
        // Step 1: Compute the fixed position of the dog on the circle
        float angle_rad = circle_angles[i] * PI / 180.0f; // Convert angle to radians
        float dog_x = cx + radius * cos(angle_rad); // Compute x position
        float dog_y = cy + radius * sin(angle_rad); // Compute y position

        // Step 2: Draw the dog, spinning in place around its center
        drawDog(dog_x, dog_y, spin_angles[i]);
    }

    glfwSwapBuffers(glfwGetCurrentContext()); // Swap buffers
}


int main() {
    loadVertices("dog.txt"); // Load dog shape

    if (vertices.empty()) {
        cerr << "Error: No vertices loaded!" << endl;
        return -1;
    }

    // Initialize GLFW
    if (!glfwInit()) {
        cerr << "GLFW Initialization Failed" << endl;
        return -1;
    }

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 800, "Rotating Dogs", NULL, NULL);
    if (!window) {
        cerr << "Window creation failed!" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Set up 2D coordinate system
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 60, 0, 60, -1, 1); // Use this instead

    glClearColor(1, 1, 1, 1); // Set background to white

    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        renderDogs();

        // Increment each dog's spin angle for in-place rotation
        for (int i = 0; i < 8; i++) {
            spin_angles[i] += rotation_speed; // Update local rotation
            //if (spin_angles[i] >= 360.0f) spin_angles[i] -= 360.0f; // Keep in range
        }
    
        glfwPollEvents(); // Process events
    }

    glfwTerminate();
    return 0;
}
