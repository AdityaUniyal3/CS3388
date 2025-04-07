#include <GL/glut.h>

// Vertex data for the star and decagon
float star[] = {0, 10, 2.5, 2.5, 10, 2.5, 4, -2.5, 7, -10, 0, -5, -7, -10, -4, -2.5, -10, 2.5, -2.5, 2.5, 0, 10};
float decagon[] = {5, 15.388, 13.0902, 9.51056, 16.1803, 0, 13.0902, -9.51056, 5, -15.388, -5, -15.388, -13.0902, -9.51056, -16.1803, 0, -13.0902, 9.51056, -5, 15.388, 5, 15.388};
float tweened[22]; // Array to store the tweened vertices

// Animation parameters
int currentFrame = 0;
int totalFrames = 400;
bool reverse = false;

void tweenVertices(float* start, float* end, float* result, float t, int numVertices) {
    for (int i = 0; i < numVertices * 2; i++) {
        result[i] = (1 - t) * start[i] + t * end[i];
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Compute interpolation factor t
    float t = (float)currentFrame / totalFrames;
    if (reverse) t = 1.0f - t;

    // Compute tweened vertices
    tweenVertices(star, decagon, tweened, t, 11);

    // Draw the tweened shape
    glColor3f(1.0, 0.5, 0.0); // Set color
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 22; i += 2) {
        glVertex2f(tweened[i], tweened[i + 1]);
    }
    glEnd();

    glutSwapBuffers();
}

void timer(int value) {
    // Update frame
    currentFrame++;
    if (currentFrame > totalFrames) {
        currentFrame = 0;
        reverse = !reverse; // Reverse direction
    }

    glutPostRedisplay(); // Trigger display update
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

void init() {
    // Set orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-20, 20, -20, 20, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutCreateWindow("OpenGL Tweening");

    init();

    glutDisplayFunc(display);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}
