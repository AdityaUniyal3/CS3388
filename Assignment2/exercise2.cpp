#include <GLFW/glfw3.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#define N_MAX 5000000
using namespace std;

static inline double frand() {
    return ((double) rand() / RAND_MAX) * 2.0 - 1.0;  // Generates number in range [-1, 1]
}

void drawDotPlot(int N) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0f, 0.0f, 0.0f);  // Black points
    glPointSize(3.0f);  // Increase point size for visibility
    glBegin(GL_POINTS);

    // Define square corners
    float corners[4][2] = { {-1.0f, -1.0f}, {1.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f, 1.0f} };

    // Pick a valid random starting corner
    int prev_corner = rand() % 4; // Store previous corner index
    float x = frand();
    float y = frand();

    for (int i = 0; i < N; ++i) {
        int cornerIndex;
        do {
            cornerIndex = rand() % 4;
        } while ((cornerIndex + 2) % 4 == prev_corner); // Ensure it's not diagonally opposite

        prev_corner = cornerIndex; // Update last chosen corner
        float cx = corners[cornerIndex][0];
        float cy = corners[cornerIndex][1];

        // Compute midpoint
        x = (x + cx) / 2.0f;
        y = (y + cy) / 2.0f;

        // Plot the point
        glVertex2f(x, y);
    }

    glEnd();
    glFlush();  // Ensure points are drawn
}

int main(int argc, char** argv) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <N> <width> <height>" << endl;
        return -1;
    }

    int N = stoi(argv[1]); // Number of points
    if (N > N_MAX) {
        cerr << "Error: N must be <= " << N_MAX << endl;
        return -1;
    }
    int width = stoi(argv[2]);
    int height = stoi(argv[3]);

    srand(static_cast<unsigned int>(time(0))); // **Initialize random seed ONCE in main**

    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(width, height, "Dot Plot", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    // Set up coordinate system
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.1, 1.1, -1.1, 1.1, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background to white

    // Rendering loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        drawDotPlot(N);  // Keep drawing the points every frame
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
