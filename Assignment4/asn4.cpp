#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "TexturedMesh.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <utility>
#include "ShaderUtils.h"

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// // Hardcoded vertices for a triangle (position: x,y,z; texture: u,v)
// std::vector<float> testVertices = {
//     -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,  // vertex 0
//      0.5f, -0.5f, 0.0f,   1.0f, 0.0f,  // vertex 1
//      0.0f,  0.5f, 0.0f,   0.5f, 1.0f   // vertex 2
// };

// // Indices for one triangle
// std::vector<GLuint> testIndices = { 0, 1, 2 };

// Input flags
bool keyUp = false, keyDown = false, keyLeft = false, keyRight = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_UP)
        keyUp = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_DOWN)
        keyDown = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_LEFT)
        keyLeft = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_RIGHT)
        keyRight = (action != GLFW_RELEASE);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed" << std::endl;
        return -1;
    }
    
    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Assignment 4", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Set the key callback
    glfwSetKeyCallback(window, key_callback);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed" << std::endl;
        return -1;
    }
    
    //glDisable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Set up a perspective projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH/HEIGHT, 0.1f, 100.0f);
    
    // Create a camera instance
    Camera camera;
    
    // Define the mesh file pairs (PLY and BMP)
    std::vector<std::pair<std::string, std::string>> meshFiles = {
        //{"LinksHouse/Floor.ply",        "LinksHouse/floor.bmp"},
        //{"LinksHouse/Patio.ply",        "LinksHouse/patio.bmp"},
        //{"LinksHouse/Walls.ply",        "LinksHouse/walls.bmp"},
        {"LinksHouse/Table.ply",        "LinksHouse/table.bmp"},
        //{"LinksHouse/WindowBG.ply",     "LinksHouse/windowbg.bmp"},
        //{"LinksHouse/WoodObjects.ply",  "LinksHouse/woodobjects.bmp"},
        //{"LinksHouse/Bottles.ply",      "LinksHouse/bottles.bmp"},
        // Transparent last:
        //{"LinksHouse/Curtains.ply",     "LinksHouse/curtains.bmp"},
        //{"LinksHouse/MetalObjects.ply", "LinksHouse/metalobjects.bmp"},
        {"LinksHouse/DoorBG.ply",       "LinksHouse/doorbg.bmp"},
    };
    
    // Create a vector to hold TexturedMesh pointers
    std::vector<TexturedMesh*> meshes;
    for (const auto &pair : meshFiles) {
        meshes.push_back(new TexturedMesh(pair.first, pair.second));
    }

    glm::vec3 globalMin( std::numeric_limits<float>::max() );
    glm::vec3 globalMax( -std::numeric_limits<float>::max() );

    for (auto mesh : meshes) {
        glm::vec3 mMin = mesh->getMinBB();
        glm::vec3 mMax = mesh->getMaxBB();
        globalMin.x = std::min(globalMin.x, mMin.x);
        globalMin.y = std::min(globalMin.y, mMin.y);
        globalMin.z = std::min(globalMin.z, mMin.z);
        globalMax.x = std::max(globalMax.x, mMax.x);
        globalMax.y = std::max(globalMax.y, mMax.y);
        globalMax.z = std::max(globalMax.z, mMax.z);
    }
    // Now compute center and radius
    //glm::vec3 sceneCenter = 0.5f * (globalMin + globalMax);
    //glm::vec3 diag = globalMax - globalMin;
    //float sceneRadius = 0.5f * glm::length(diag);
    //glm::vec3 cameraPos = sceneCenter + glm::vec3(0.0f, 0.0f, sceneRadius * 1.5f);
    // // If you want to be a bit above the scene:
    // cameraPos.y += 2.0f;  // e.g. raise camera 2 units
    //camera.setPosition(glm::vec3(0.0f, 2.0f, 12.0f));
    // // If your camera faces negative Z when yaw=180, set that:
    //camera.setYaw(180.0f);
    // // For debugging:
    // std::cout << "Global BB:\n"
    //           << " min(" << globalMin.x << ", " << globalMin.y << ", " << globalMin.z << ")\n"
    //           << " max(" << globalMax.x << ", " << globalMax.y << ", " << globalMax.z << ")\n"
    //           << " center(" << sceneCenter.x << ", " << sceneCenter.y << ", " << sceneCenter.z << ")\n"
    //           << " radius = " << sceneRadius << "\n";

    // GLuint testVAO, testVBO, testEBO;
    // glGenVertexArrays(1, &testVAO);
    // glGenBuffers(1, &testVBO);
    // glGenBuffers(1, &testEBO);
    
    // glBindVertexArray(testVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, testVBO);
    // glBufferData(GL_ARRAY_BUFFER, testVertices.size() * sizeof(float), testVertices.data(), GL_STATIC_DRAW);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, testEBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, testIndices.size() * sizeof(GLuint), testIndices.data(), GL_STATIC_DRAW);
    
    // // Assuming the test shader uses location 0 for position.
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // // (We don't need texture coordinates for a constant-color shader, but if you include them:)
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    // glBindVertexArray(0);    

    // // Create a simple shader program for the test geometry (constant red color)
    // const char* testVertexShaderSource = R"(
    // #version 120
    // attribute vec3 inPos;
    // uniform mat4 MVP;
    // void main() {
    //     gl_Position = MVP * vec4(inPos, 1.0);
    // }
    // )";
    
    // const char* testFragmentShaderSource = R"(
    // #version 120
    // void main() {
    //     gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);  // red color
    // }
    // )";

    // GLuint testVS = compileShader(GL_VERTEX_SHADER, testVertexShaderSource);
    // GLuint testFS = compileShader(GL_FRAGMENT_SHADER, testFragmentShaderSource);
    // GLuint testShaderProgram = glCreateProgram();
    // glAttachShader(testShaderProgram, testVS);
    // glAttachShader(testShaderProgram, testFS);
    // glBindAttribLocation(testShaderProgram, 0, "inPos");
    // glLinkProgram(testShaderProgram);
    // glDeleteShader(testVS);
    // glDeleteShader(testFS);
    
    
    // Timing
    float lastTime = glfwGetTime();
    
    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Update camera based on arrow key input
        camera.update(keyUp, keyDown, keyLeft, keyRight, deltaTime);
        //std::cout << camera.getDebugInfo() << std::endl;
        glm::mat4 view = camera.getViewMatrix();

        // Clear the screen (set to a non-black color temporarily if needed for debugging)
        glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        //glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -10.0f));
        //model = glm::scale(model, glm::vec3(3.0f));
        glm::mat4 mvp = projection * view * model;


        // // Draw the test triangle first
        // glUseProgram(testShaderProgram);
        // GLint testMvpLoc = glGetUniformLocation(testShaderProgram, "MVP");
        // glUniformMatrix4fv(testMvpLoc, 1, GL_FALSE, &mvp[0][0]);
        // glBindVertexArray(testVAO);
        // glDrawElements(GL_TRIANGLES, testIndices.size(), GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0);
        // glUseProgram(0);
        //glDisable(GL_DEPTH_TEST);
        //Draw each mesh with the computed MVP matrix.
        for (auto mesh : meshes) {
            mesh->draw(&mvp[0][0]);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Clean up: delete mesh instances
    for (auto mesh : meshes) {
        delete mesh;
    }
    // glDeleteVertexArrays(1, &testVAO);
    // glDeleteBuffers(1, &testVBO);
    // glDeleteBuffers(1, &testEBO);
    // glDeleteProgram(testShaderProgram);
    
    glfwTerminate();
    return 0;
}