#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <iostream>

//------------------------------------------------------------------------------------
// Data Structures
//------------------------------------------------------------------------------------

// A simple 2D point structure.
struct Point {
    float x, y;
};

// A Node represents a point on the spline and may have two control handles.
// The control handles are used for constructing the cubic Bezier curves.
struct Node : Point {
    bool hasHandle1 = false; // Indicates if the node has the first handle.
    bool hasHandle2 = false; // Indicates if the node has the second handle.
    Point handle1;         // Control handle for the incoming curve segment.
    Point handle2;         // Control handle for the outgoing curve segment.
};

//------------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------------

// Container for all nodes of the spline. The order in this vector defines the spline order.
static std::vector<Node> g_nodes;

// Initial logical window dimensions.
static int g_winWidth  = 800;
static int g_winHeight = 600;

// Variables to support dragging of nodes or handles.
// g_isDragging: true when a node/handle is being dragged.
// g_draggedNodeIndex: index of the node being dragged.
// g_draggedHandleIndex: if dragging a handle, 1 or 2; -1 if dragging the entire node.
// g_offsetX and g_offsetY: store the offset between the cursor and the node's position when dragging starts.
static bool  g_isDragging          = false;
static int   g_draggedNodeIndex    = -1;
static int   g_draggedHandleIndex  = -1;
static float g_offsetX = 0.0f, g_offsetY = 0.0f;

//------------------------------------------------------------------------------------
// Utility Functions
//------------------------------------------------------------------------------------

// Returns the squared distance between two points (used for efficient comparisons).
float distSq(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return dx * dx + dy * dy;
}

// Returns the Euclidean distance between two points.
float dist(float x1, float y1, float x2, float y2) {
    return std::sqrt(distSq(x1, y1, x2, y2));
}

// Evaluate a cubic Bezier curve at parameter t (0 <= t <= 1).
// p0: start point, p1: first control point, p2: second control point, p3: end point.
Point cubicBezier(const Point& p0, const Point& p1, const Point& p2, const Point& p3, float t)
{
    float u   = 1.0f - t;
    float tt  = t * t;
    float uu  = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    // The cubic Bezier formula.
    Point res;
    res.x = uuu * p0.x + 3.0f * uu * t * p1.x + 3.0f * u * tt * p2.x + ttt * p3.x;
    res.y = uuu * p0.y + 3.0f * uu * t * p1.y + 3.0f * u * tt * p2.y + ttt * p3.y;
    return res;
}

// Adjusts the second control handle so that it is a mirror of the first handle relative to the node.
// This enforces symmetry for interior nodes.
void enforceSymmetry(Node& node, bool handle1Moved)
{
    // Only enforce symmetry if both handles are present.
    if (!node.hasHandle1 || !node.hasHandle2) {
        return;
    }
    float dx, dy;
    if (handle1Moved) {
        // Compute the vector from the node to handle1.
        dx = node.x - node.handle1.x;
        dy = node.y - node.handle1.y;
        // Set handle2 to be the mirror of handle1.
        node.handle2.x = node.x + dx;
        node.handle2.y = node.y + dy;
    } else {
        // Compute the vector from the node to handle2.
        dx = node.x - node.handle2.x;
        dy = node.y - node.handle2.y;
        // Set handle1 to be the mirror of handle2.
        node.handle1.x = node.x + dx;
        node.handle1.y = node.y + dy;
    }
}

// Creates a new node at (x, y) with a control handle 50 pixels above it.
// Also finds the closest existing endpoint and converts it into an interior node by adding a second handle.
void createNode(float x, float y)
{
    // Create the new node (endpoint) with one control handle.
    Node newNode;
    newNode.x = x;
    newNode.y = y;
    newNode.hasHandle1 = true;
    newNode.hasHandle2 = false;
    newNode.handle1.x = x;
    newNode.handle1.y = y + 50.0f; // Place handle 50 pixels above.
    g_nodes.push_back(newNode);

    // If this is the first node, no connection is possible yet.
    if (g_nodes.size() == 1) {
        return;
    }

    // Find the closest endpoint among the existing nodes (excluding the newly added node).
    float minDist = 1e9f;
    int closestIndex = -1;
    for (int i = 0; i < (int)g_nodes.size() - 1; i++) {
        Node& n = g_nodes[i];
        // A node is considered an endpoint if it does not have both handles.
        bool isEndpoint = (!n.hasHandle1 || !n.hasHandle2);
        if (!isEndpoint)
            continue;
        float d = dist(n.x, n.y, x, y);
        if (d < minDist) {
            minDist = d;
            closestIndex = i;
        }
    }
    if (closestIndex < 0)
        return;
    Node& cNode = g_nodes[closestIndex];
    
    // Convert the closest endpoint into an interior node by adding the missing handle.
    if (!cNode.hasHandle1 && !cNode.hasHandle2) {
        cNode.hasHandle1 = true;
        cNode.hasHandle2 = true;
        cNode.handle1.x = cNode.x - 50.0f;
        cNode.handle1.y = cNode.y;
        cNode.handle2.x = cNode.x + 50.0f;
        cNode.handle2.y = cNode.y;
    } else if (cNode.hasHandle1 && !cNode.hasHandle2) {
        cNode.hasHandle2 = true;
        float dx = cNode.x - cNode.handle1.x;
        float dy = cNode.y - cNode.handle1.y;
        cNode.handle2.x = cNode.x + dx;
        cNode.handle2.y = cNode.y + dy;
    } else if (!cNode.hasHandle1 && cNode.hasHandle2) {
        cNode.hasHandle1 = true;
        float dx = cNode.x - cNode.handle2.x;
        float dy = cNode.y - cNode.handle2.y;
        cNode.handle1.x = cNode.x + dx;
        cNode.handle1.y = cNode.y + dy;
    }
}

// Clears all nodes and resets dragging state.
void resetScene()
{
    g_nodes.clear();
    g_isDragging = false;
    g_draggedNodeIndex = -1;
    g_draggedHandleIndex = -1;
}

//------------------------------------------------------------------------------------
// "Dotted Line" Rendering Helper
//------------------------------------------------------------------------------------
// Draws a dotted (dashed) line from 'start' to 'end' using short dashes and gaps.
void drawDottedLine(const Point& start, const Point& end, float dashLen = 5.f, float gapLen = 5.f)
{
    // Calculate total length and the angle of the line.
    float length = dist(start.x, start.y, end.x, end.y);
    float dx = end.x - start.x;
    float dy = end.y - start.y;
    float angle = std::atan2(dy, dx);
    float traveled = 0.0f;
    float cx = start.x; // Current starting x coordinate for the dash.
    float cy = start.y; // Current starting y coordinate for the dash.

    glBegin(GL_LINES);
    while (traveled < length) {
        // Determine where the dash ends.
        float dashEnd = traveled + dashLen;
        if (dashEnd > length) dashEnd = length;
        // Calculate end coordinates of the dash.
        float dashX = start.x + std::cos(angle) * dashEnd;
        float dashY = start.y + std::sin(angle) * dashEnd;
        glVertex2f(cx, cy);
        glVertex2f(dashX, dashY);
        // Advance by dash plus gap.
        traveled = dashEnd + gapLen;
        cx = start.x + std::cos(angle) * traveled;
        cy = start.y + std::sin(angle) * traveled;
    }
    glEnd();
}

//------------------------------------------------------------------------------------
// Rendering Functions
//------------------------------------------------------------------------------------
// Draws the entire scene (spline, nodes, control points, and dotted lines) to the screen.
void renderScene()
{
    // Clear the background to white.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Enable line smoothing and blending for anti-aliasing.
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);

    // Draw cubic Bezier curves connecting each consecutive pair of nodes.
    glColor3f(0.0f, 0.0f, 0.0f); // Set curve color to black.
    for (int i = 0; i < (int)g_nodes.size() - 1; i++) {
        Node& n0 = g_nodes[i];
        Node& n1 = g_nodes[i+1];
        // p0 is the start point, p1 is the outgoing control point,
        // p2 is the incoming control point of the next node, and p3 is the end point.
        Point p0 = { n0.x, n0.y };
        Point p1 = (n0.hasHandle2) ? n0.handle2 : p0;
        Point p2 = (n1.hasHandle1) ? n1.handle1 : Point{ n1.x, n1.y };
        Point p3 = { n1.x, n1.y };
        const int NUM_SEGMENTS = 200; // More segments give a smoother curve.
        glBegin(GL_LINE_STRIP);
        for (int seg = 0; seg <= NUM_SEGMENTS; seg++) {
            float t = seg / (float)NUM_SEGMENTS;
            Point pt = cubicBezier(p0, p1, p2, p3, t);
            glVertex2f(pt.x, pt.y);
        }
        glEnd();
    }

    // Render each node as a blue square.
    glPointSize(10.0f);
    glDisable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
    glColor3f(0.0f, 0.0f, 1.0f); // Blue color for nodes.
    for (auto & node : g_nodes) {
        glVertex2f(node.x, node.y);
    }
    glEnd();

    // Render each control point as a black circle.
    glEnable(GL_POINT_SMOOTH);
    glPointSize(8.0f);
    glColor3f(0.0f, 0.0f, 0.0f); // Black color for control points.
    glBegin(GL_POINTS);
    for (auto & node : g_nodes) {
        if (node.hasHandle1)
            glVertex2f(node.handle1.x, node.handle1.y);
        if (node.hasHandle2)
            glVertex2f(node.handle2.x, node.handle2.y);
    }
    glEnd();

    // Render dotted lines from each node to its control handles.
    glColor3f(0.5, 0.7, 1.0); // A light blue for the dotted lines.
    for (auto & node : g_nodes) {
        Point pNode = { node.x, node.y };
        if (node.hasHandle1)
            drawDottedLine(pNode, node.handle1, 4.0f, 4.0f);
        if (node.hasHandle2)
            drawDottedLine(pNode, node.handle2, 4.0f, 4.0f);
    }
}

//------------------------------------------------------------------------------------
// Input Handling Functions
//------------------------------------------------------------------------------------

// Helper function to invert the Y coordinate.
// 'y' is the mouse coordinate in framebuffer space and we subtract it from the framebuffer height.
float invertY(float y, int fbHeight) {
    return (float)fbHeight - y;
}

// Mouse button callback to handle clicking and dragging.
// Uses scaling to convert mouse coordinates (logical) to framebuffer coordinates.
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            // Query both the logical window size and the actual framebuffer size.
            int winW, winH;
            glfwGetWindowSize(window, &winW, &winH);
            int fbW, fbH;
            glfwGetFramebufferSize(window, &fbW, &fbH);

            // Get the current mouse position (in logical window coordinates).
            double mx, my;
            glfwGetCursorPos(window, &mx, &my);
            // Calculate scale factors from logical size to framebuffer size.
            float scaleX = (float)fbW / winW;
            float scaleY = (float)fbH / winH;
            // Scale the mouse coordinates and invert Y.
            float fx = (float)mx * scaleX;
            float fy = invertY((float)my * scaleY, fbH);

            // Use a small radius for picking nodes/handles.
            const float pickRadius = 10.0f;
            const float pickRadiusSq = pickRadius * pickRadius;

            bool foundHit = false;
            // Check if the click is on any node or control handle.
            for (int i = 0; i < (int)g_nodes.size(); i++) {
                Node & n = g_nodes[i];
                if (distSq(fx, fy, n.x, n.y) <= pickRadiusSq) {
                    g_isDragging = true;
                    g_draggedNodeIndex = i;
                    g_draggedHandleIndex = -1; // dragging the node itself.
                    g_offsetX = n.x - fx;
                    g_offsetY = n.y - fy;
                    foundHit = true;
                    break;
                }
                if (n.hasHandle1 && distSq(fx, fy, n.handle1.x, n.handle1.y) <= pickRadiusSq) {
                    g_isDragging = true;
                    g_draggedNodeIndex = i;
                    g_draggedHandleIndex = 1; // dragging handle1.
                    g_offsetX = n.handle1.x - fx;
                    g_offsetY = n.handle1.y - fy;
                    foundHit = true;
                    break;
                }
                if (n.hasHandle2 && distSq(fx, fy, n.handle2.x, n.handle2.y) <= pickRadiusSq) {
                    g_isDragging = true;
                    g_draggedNodeIndex = i;
                    g_draggedHandleIndex = 2; // dragging handle2.
                    g_offsetX = n.handle2.x - fx;
                    g_offsetY = n.handle2.y - fy;
                    foundHit = true;
                    break;
                }
            }
            // If nothing was hit, create a new node at the click location.
            if (!foundHit) {
                createNode(fx, fy);
            }
        }
        else if (action == GLFW_RELEASE) {
            // On mouse release, clear dragging state.
            g_isDragging = false;
            g_draggedNodeIndex = -1;
            g_draggedHandleIndex = -1;
        }
    }
}

// Mouse move callback to handle dragging of nodes or handles.
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (g_isDragging && g_draggedNodeIndex >= 0 && g_draggedNodeIndex < (int)g_nodes.size()) {
        // Get window and framebuffer sizes.
        int winW, winH;
        glfwGetWindowSize(window, &winW, &winH);
        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        // Compute scale factors.
        float scaleX = (float)fbW / winW;
        float scaleY = (float)fbH / winH;
        // Convert mouse position to framebuffer coordinates.
        float fx = (float)xpos * scaleX;
        float fy = invertY((float)ypos * scaleY, fbH);

        Node & n = g_nodes[g_draggedNodeIndex];
        if (g_draggedHandleIndex == -1) {
            // Dragging the entire node.
            float oldX = n.x, oldY = n.y;
            n.x = fx + g_offsetX;
            n.y = fy + g_offsetY;
            // Move any control handles along with the node.
            float dx = n.x - oldX;
            float dy = n.y - oldY;
            if (n.hasHandle1) {
                n.handle1.x += dx;
                n.handle1.y += dy;
            }
            if (n.hasHandle2) {
                n.handle2.x += dx;
                n.handle2.y += dy;
            }
        } else {
            // Dragging a control handle.
            if (g_draggedHandleIndex == 1) {
                n.handle1.x = fx + g_offsetX;
                n.handle1.y = fy + g_offsetY;
                // If the node has two handles, enforce symmetry.
                if (n.hasHandle1 && n.hasHandle2)
                    enforceSymmetry(n, true);
            } else { // g_draggedHandleIndex == 2
                n.handle2.x = fx + g_offsetX;
                n.handle2.y = fy + g_offsetY;
                if (n.hasHandle1 && n.hasHandle2)
                    enforceSymmetry(n, false);
            }
        }
    }
}

// Checks for keyboard input; resets the scene when 'E' is pressed.
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        resetScene();
    }
}

//------------------------------------------------------------------------------------
// Main Function
//------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    // Optional: override default window dimensions with command-line arguments.
    if (argc >= 3) {
        g_winWidth  = std::stoi(argv[1]);
        g_winHeight = std::stoi(argv[2]);
    }

    if (!glfwInit()) {
        std::cerr << "Failed to init GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // Enable 4x multisampling for anti-aliasing.

    // Create a GLFW window with the specified logical dimensions.
    GLFWwindow* window = glfwCreateWindow(g_winWidth, g_winHeight,
                                          "Bezier Spline (CS3388 Assignment 3)",
                                          nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Set the mouse button and cursor position callbacks.
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);

    // Main loop.
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Get the current framebuffer size.
        int fbW, fbH;
        glfwGetFramebufferSize(window, &fbW, &fbH);
        // Update the viewport and projection to match the framebuffer size.
        glViewport(0, 0, fbW, fbH);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, fbW, 0.0, fbH, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Render the scene.
        renderScene();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
