
#include "PlaneMesh.hpp"
#include "ShaderLoader.hpp"
#include <iostream>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

PlaneMesh::PlaneMesh(float min, float max, float stepsize) {
    this->min = min;
    this->max = max;
    modelColor = glm::vec4(0, 1.0f, 1.0f, 1.0f);

    planeMeshQuads(min, max, stepsize);
    numVerts = verts.size() / 3;
    numIndices = indices.size();

	// Load shader program here!
    // shaderProgram = LoadShaders(
    //     "shaders/WaterShader.vertexshader",
    //     "shaders/WaterShader.tcs",
    //     "shaders/WaterShader.tes",
    //     "shaders/WaterShader.geoshader",
    //     "shaders/WaterShader.fragmentshader"
    // );

    shaderProgram = LoadShaders("shaders/debug.vert", "shaders/WaterShader.tcs", "shaders/WaterShader.tes", "", "shaders/debug.frag");

    outerTessLoc = glGetUniformLocation(shaderProgram, "outerTess");
    innerTessLoc = glGetUniformLocation(shaderProgram, "innerTess");

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Vertex positions
    glGenBuffers(1, &vboVerts);
    glBindBuffer(GL_ARRAY_BUFFER, vboVerts);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Normals
    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Indices
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void PlaneMesh::planeMeshQuads(float min, float max, float stepsize) {
    float x = min;
    float y = 0;
    for (float z = min; z <= max; z += stepsize) {
        verts.push_back(x);
        verts.push_back(y);
        verts.push_back(z);
        normals.push_back(0);
        normals.push_back(1);
        normals.push_back(0);
    }

    for (float x = min + stepsize; x <= max; x += stepsize) {
        for (float z = min; z <= max; z += stepsize) {
            verts.push_back(x);
            verts.push_back(y);
            verts.push_back(z);
            normals.push_back(0);
            normals.push_back(1);
            normals.push_back(0);
        }
    }

    int nCols = (max - min) / stepsize + 1;
    int i = 0, j = 0;
    for (float x = min; x < max; x += stepsize) {
        j = 0;
        for (float z = min; z < max; z += stepsize) {
            indices.push_back(i * nCols + j);
            indices.push_back(i * nCols + j + 1);
            indices.push_back((i + 1) * nCols + j + 1);
            indices.push_back((i + 1) * nCols + j);
            ++j;
        }
        ++i;
    }
}

void PlaneMesh::draw(glm::vec3 lightPos, glm::mat4 V, glm::mat4 P) {
    glUseProgram(shaderProgram);

    glUniform1f(outerTessLoc, 16.0f); // As required by assignment
    glUniform1f(innerTessLoc, 16.0f);

    glm::mat4 M = glm::mat4(1.0f);
    glm::mat4 MVP = P * V * M;

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "M"), 1, GL_FALSE, glm::value_ptr(M));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "V"), 1, GL_FALSE, glm::value_ptr(V));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "P"), 1, GL_FALSE, glm::value_ptr(P));

    glm::vec3 eyePos = glm::vec3(glm::inverse(V)[3]); // Extract camera position from view matrix
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
    glUniform3fv(glGetUniformLocation(shaderProgram, "eyePos"), 1, glm::value_ptr(eyePos));
    // GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos_worldspace");
    // glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

    // GLuint timeLoc = glGetUniformLocation(shaderProgram, "time");
    // glUniform1f(timeLoc, (float)glfwGetTime());

    // glBindVertexArray(vao);
    // glPatchParameteri(GL_PATCH_VERTICES, 4);
    // glDrawElements(GL_PATCHES, numIndices, GL_UNSIGNED_INT, 0);
    // glBindVertexArray(0);
    glBindVertexArray(vao);
    //std::cout << "Drawing PlaneMesh with " << numIndices << " indices." << std::endl;
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // Wireframe mode
    glDrawElements(GL_PATCHES, numIndices, GL_UNSIGNED_INT, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Restore
    glBindVertexArray(0);
}
