#define GLM_ENABLE_EXPERIMENTAL

#include <iostream>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>

// OpenGL, GLFW and GLEW
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM for matrix math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Include our modules
#include "marching_cubes.h"
#include "compute_normals.h"
#include "write_ply.h"
#include "camera.h"
#include "shader_utils.h"

//function for x2−y2−z2−z with an isovalue of -1.5
float myFunction1(float x, float y, float z) {
    return x*x-y*y-z*z-z;
}
//y − sin(x)cos(z) with an isovalue of 0
float myFunction2(float x, float y, float z) {
    return y - sin(x)*cos(z);
}
void drawBox() {

    // Define the min and max corners of the marching volume
    float min_corner[3] = {-5, -5, -5};
    float max_corner[3] = {5.2, 5.2, 5.2};

    glLineWidth(2.0);
    glColor3f(1.0, 1.0, 1.0);  // Set color to white
    glBegin(GL_LINES);
    // Bottom rectangle
    glVertex3f(min_corner[0], min_corner[1], min_corner[2]);
    glVertex3f(max_corner[0], min_corner[1], min_corner[2]);

    glVertex3f(max_corner[0], min_corner[1], min_corner[2]);
    glVertex3f(max_corner[0], min_corner[1], max_corner[2]);

    glVertex3f(max_corner[0], min_corner[1], max_corner[2]);
    glVertex3f(min_corner[0], min_corner[1], max_corner[2]);

    glVertex3f(min_corner[0], min_corner[1], max_corner[2]);
    glVertex3f(min_corner[0], min_corner[1], min_corner[2]);

    // Top rectangle
    glVertex3f(min_corner[0], max_corner[1], min_corner[2]);
    glVertex3f(max_corner[0], max_corner[1], min_corner[2]);

    glVertex3f(max_corner[0], max_corner[1], min_corner[2]);
    glVertex3f(max_corner[0], max_corner[1], max_corner[2]);

    glVertex3f(max_corner[0], max_corner[1], max_corner[2]);
    glVertex3f(min_corner[0], max_corner[1], max_corner[2]);

    glVertex3f(min_corner[0], max_corner[1], max_corner[2]);
    glVertex3f(min_corner[0], max_corner[1], min_corner[2]);

    // Connect the corresponding corners
    glVertex3f(min_corner[0], min_corner[1], min_corner[2]);
    glVertex3f(min_corner[0], max_corner[1], min_corner[2]);

    glVertex3f(max_corner[0], min_corner[1], min_corner[2]);
    glVertex3f(max_corner[0], max_corner[1], min_corner[2]);

    glVertex3f(max_corner[0], min_corner[1], max_corner[2]);
    glVertex3f(max_corner[0], max_corner[1], max_corner[2]);

    glVertex3f(min_corner[0], min_corner[1], max_corner[2]);
    glVertex3f(min_corner[0], max_corner[1], max_corner[2]);

    glEnd();
}

void drawAxes() {
    // Define the min and max corners of the marching volume
    float min_corner[3] = {-5, -5, -5};
    float max_corner[3] = {5.3, 5.3, 5.3};

    // Draw axes as lines
    glLineWidth(5.0);
    glBegin(GL_LINES);
    // X-axis (Red)
    glColor3f(1.0, 0.0, 0.0); // Red
    glVertex3f(min_corner[0], min_corner[1], min_corner[2]);
    glVertex3f(max_corner[0], min_corner[1], min_corner[2]);

    // Y-axis (Green)
    glColor3f(0.0, 1.0, 0.0); // Green
    glVertex3f(min_corner[0], min_corner[1], min_corner[2]);
    glVertex3f(min_corner[0], max_corner[1], min_corner[2]);

    // Z-axis (Blue)
    glColor3f(0.0, 0.0, 1.0); // Blue
    glVertex3f(min_corner[0], min_corner[1], min_corner[2]);
    glVertex3f(min_corner[0], min_corner[1], max_corner[2]);
    glEnd();

    // Parameters for small filled arrowheads
    float arrowLength = 0.25f;     // Distance from base to tip
    float arrowHalfSize = 0.1f;  // Half the width of the base

    // Draw filled arrowhead for X-axis
    glBegin(GL_TRIANGLES);
    glColor3f(1.0, 0.0, 0.0); // Red
    // Tip of arrow
    glVertex3f(max_corner[0] + arrowLength, min_corner[1], min_corner[2]);
    // Base vertices of arrowhead
    glVertex3f(max_corner[0], min_corner[1] + arrowHalfSize, min_corner[2]);
    glVertex3f(max_corner[0], min_corner[1] - arrowHalfSize, min_corner[2]);
    glEnd();

    // Draw filled arrowhead for Y-axis
    glBegin(GL_TRIANGLES);
    glColor3f(0.0, 1.0, 0.0); // Green
    // Tip of arrow
    glVertex3f(min_corner[0], max_corner[1] + arrowLength, min_corner[2]);
    // Base vertices of arrowhead
    glVertex3f(min_corner[0] + arrowHalfSize, max_corner[1], min_corner[2]);
    glVertex3f(min_corner[0] - arrowHalfSize, max_corner[1], min_corner[2]);
    glEnd();

    // Draw filled arrowhead for Z-axis
    glBegin(GL_TRIANGLES);
    glColor3f(0.0, 0.0, 1.0); // Blue
    // Tip of arrow
    glVertex3f(min_corner[0], min_corner[1], max_corner[2] + arrowLength);
    // Base vertices of arrowhead
    glVertex3f(min_corner[0], min_corner[1] + arrowHalfSize, max_corner[2]);
    glVertex3f(min_corner[0], min_corner[1] - arrowHalfSize, max_corner[2]);
    glEnd();
}

// Global variable for our VAO
GLuint VAO = 0;
GLuint meshVBO = 0;

// Helper: convert vector<float> data (positions and normals) into vector<Vertex>
std::vector<Vertex> createMesh(const std::vector<float>& positions, const std::vector<float>& normals) {
    std::vector<Vertex> mesh;
    size_t vertexCount = positions.size() / 3;
    for (size_t i = 0; i < vertexCount; ++i) {
        Vertex v;
        v.x = positions[i * 3 + 0];
        v.y = positions[i * 3 + 1];
        v.z = positions[i * 3 + 2];
        v.nx = normals[i * 3 + 0];
        v.ny = normals[i * 3 + 1];
        v.nz = normals[i * 3 + 2];
        mesh.push_back(v);
    }
    return mesh;
}

// Helper: Bind mesh data (vector<Vertex>) to a VAO and VBO for rendering
void bindMesh(const std::vector<Vertex>& mesh, GLuint &VAO) {
    GLuint VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.size() * sizeof(Vertex), mesh.data(), GL_STATIC_DRAW);
    
    // Attribute 0: Position (vec3), starting at offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Attribute 1: Normal (vec3), starting at offset of nx
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, nx));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

int main(int argc, char* argv[]) {
    // Default parameters
    float screenW = 800;
    float screenH = 600;
    float stepsize = 0.4f;
    if (argc > 1) screenW = atof(argv[1]);
    if (argc > 2) screenH = atof(argv[2]);
    if (argc > 3) stepsize = atof(argv[3]);
    
    // Initialize GLFW
    if (!glfwInit()){
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    // Create GLFW window and OpenGL context
    GLFWwindow* window = glfwCreateWindow(screenW, screenH, "Assignment 5", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // After creating window:
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK){
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    // Set input mode
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // Set background and enable depth testing
    glClearColor(0.2f, 0.2f, 0.3f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // Set up projection and view matrices using GLM
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), screenW/screenH, 0.1f, 1000.0f);
    glm::vec3 eye(0.0f, 3.0f, 5.0f);
    glm::vec3 center(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::mat4 V = glm::lookAt(eye, center, up);
    
    // Compile shader program
    GLuint shaderProgram = compileShaderProgram();
    
    // Generate mesh from the scalar field (using the sphere function)
    float min_bound = -5.0f, max_bound = 5.0f;
    std::vector<float> positions = marching_cubes(myFunction1, -1.5, min_bound, max_bound, stepsize);
    std::vector<float> normals = compute_normals(positions);
    // Export mesh for inspection (optional)
    writePLY(positions, normals, "output_mesh.ply");
    
    // Convert positions and normals into a vector of Vertex structs
    std::vector<Vertex> mesh = createMesh(positions, normals);
    
    // Bind mesh data into a VAO for shader-based rendering
    bindMesh(mesh, VAO);
    
    // Main render loop
    do {
        // Update camera using first-person controls (this updates the view matrix V)
        cameraFirstPerson(window, V, 10.0f);
        
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use our shader program and update uniform matrices
        glUseProgram(shaderProgram);
        // Retrieve uniform locations
        GLint mvpLoc       = glGetUniformLocation(shaderProgram, "MVP");
        GLint mLoc         = glGetUniformLocation(shaderProgram, "M");
        GLint vLoc         = glGetUniformLocation(shaderProgram, "V");
        GLint normalMatrixLoc  = glGetUniformLocation(shaderProgram, "normalMatrix");
        GLint lightDirLoc  = glGetUniformLocation(shaderProgram, "LightDir");
        GLint camPosLoc    = glGetUniformLocation(shaderProgram, "cameraPos");
        GLint modelColLoc  = glGetUniformLocation(shaderProgram, "modelColor");
        GLint ambColLoc    = glGetUniformLocation(shaderProgram, "ambientColor");
        GLint specColLoc   = glGetUniformLocation(shaderProgram, "specularColor");
        GLint shininessLoc = glGetUniformLocation(shaderProgram, "shininess");

        // Suppose your model transform is identity
        glm::mat4 M = glm::mat4(1.0f);
        // Suppose your camera is in 'eye', view matrix is 'V', projection is 'Projection'
        glm::mat4 MVP = Projection * V * M;
        // Compute the normal matrix from M
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(M)));

        // Upload them
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
        glUniformMatrix4fv(mLoc,   1, GL_FALSE, glm::value_ptr(M));
        glUniformMatrix4fv(vLoc,   1, GL_FALSE, glm::value_ptr(V));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));


        // Example lighting data
        glUniform3f(lightDirLoc, 0.0f, -1.0f, -1.0f);
        glUniform3f(camPosLoc,   eye.x, eye.y, eye.z);
        glUniform3f(modelColLoc, 0.0f, 0.8f, 0.8f);
        glUniform3f(ambColLoc,   0.2f, 0.2f, 0.2f);
        glUniform3f(specColLoc,  1.0f, 1.0f, 1.0f);
        glUniform1f(shininessLoc, 64.0f);

        // Now bind your VAO and draw
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, mesh.size());
        glBindVertexArray(0);
        
        // Now switch to fixed-function mode
        glUseProgram(0);

        // Update fixed-function pipeline's matrices
        glMatrixMode(GL_PROJECTION);
        glLoadMatrixf(glm::value_ptr(Projection));
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(glm::value_ptr(V));

        drawAxes();
        drawBox();
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    } while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );
    
    // Cleanup: delete VAO (and any other buffers if needed)
    glDeleteVertexArrays(1, &VAO);
    
    glfwTerminate();
    return 0;
}
